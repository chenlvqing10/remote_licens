#include <linux/platform_device.h>

#include <drm/drmP.h>
#include <drm/lombo_drm.h>

#include "lombo_subdrv.h"
#include "lombo_se.h"

enum lombo_scale_client_state {
	LOMBO_SCALE_CLIENT_STATE_IDLE = 0,
	LOMBO_SCALE_CLIENT_STATE_WAIT2START,
	LOMBO_SCALE_CLIENT_STATE_PREPARE, /* can apply exec arg to rot */
	LOMBO_SCALE_CLIENT_STATE_WAIT4FINISH,
	LOMBO_SCALE_CLIENT_STATE_FINISH = LOMBO_SCALE_CLIENT_STATE_IDLE,
};

enum lombo_scale_state {
	LOMBO_SCALE_STATE_IDLE = 0,
	LOMBO_SCALE_STATE_BUSSY,
};

struct lombo_se;

struct lombo_scale_client {
	struct device *dev;
	struct list_head file_node; /* node in file private */
	uint32_t id; /* id in idr */

	uint64_t timeout; /* ns */

	struct list_head work_node;
	enum lombo_scale_client_state state;
	wait_queue_head_t wait;

	spinlock_t wlock;
	int wlocked;

};

struct lombo_scale {
	struct lombo_subdrv subdrv;

	struct mutex se_mutex;
	uint32_t client_cnt;
	struct lombo_se *se;

	/** Mapping of client handles to client pointers. */
	struct idr client_idr;
	/** Lock for synchronization of access to client_idr. */
	spinlock_t idr_lock;

	spinlock_t work_lock;
	enum lombo_scale_state state;
	struct list_head work_list; /* work list head */
};

struct lombo_scale_file_private {
	struct lombo_scale *scale;
	spinlock_t client_slock; /* for protect client_list */
	struct list_head client_list;
};

int lombo_scale_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_scale_file_private *scale_fpriv = file_priv->scale_fpriv;
	struct lombo_scale *scale;
	struct lombo_scale_client *client;
	drm_lombo_scale_user_handle_t *handle = data;
	int ret;

	if (!scale_fpriv) {
		DRM_ERROR("no scale file priv\n");
		return -ENOENT;
	}
	scale = scale_fpriv->scale;

	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		DRM_ERROR("failed to alloc for scale client\n");
		return -ENOMEM;
	}
	client->dev = scale->subdrv.dev;

	idr_preload(GFP_KERNEL);
	spin_lock(&scale->idr_lock);
	ret = idr_alloc(&scale->client_idr, client, 1, 0, GFP_NOWAIT);
	spin_unlock(&scale->idr_lock);
	idr_preload_end();
	if (ret < 1) {
		DRM_ERROR("failed to idr alloc for rot client, ret=%d\n", ret);
		kfree(client);
		return ret ? ret : -1;
	}
	client->id = ret;
	*handle = ret;

	client->timeout = 3000000000ULL; /* 3 second */

	INIT_LIST_HEAD(&client->work_node);
	init_waitqueue_head(&client->wait);
	spin_lock_init(&client->wlock);
	client->wlocked = 0;

	INIT_LIST_HEAD(&client->file_node);
	spin_lock(&scale_fpriv->client_slock);
	list_add_tail(&client->file_node, &scale_fpriv->client_list);
	spin_unlock(&scale_fpriv->client_slock);

	ret = 0;
	mutex_lock(&scale->se_mutex);
	scale->client_cnt++;
	if (scale->client_cnt == 1)
		scale->se = lombo_se_request(drm, scale);
	if (!scale->se)
		ret = -ENODEV;
	mutex_unlock(&scale->se_mutex);

	if (ret) {
		idr_remove(&scale->client_idr, *handle);
		*handle = 0;
		kfree(client);
		DRM_ERROR("%s fail\n", __func__);
	}

	return ret;
}

int lombo_scale_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_scale_file_private *scale_fpriv = file_priv->scale_fpriv;
	drm_lombo_scale_user_handle_t *handle = data;
	struct lombo_scale *scale;
	struct lombo_scale_client *client;

	if (!scale_fpriv) {
		DRM_ERROR("no scale file priv\n");
		return -ENOENT;
	}
	scale = scale_fpriv->scale;

	spin_lock(&scale->idr_lock);
	client = idr_find(&scale->client_idr, *handle);
	if (client == NULL) {
		spin_unlock(&scale->idr_lock);
		DRM_INFO("%d: fail scale close. handle=%d\n",
			__LINE__, *handle);
		return -EINVAL;
	}
	idr_remove(&scale->client_idr, *handle);
	spin_unlock(&scale->idr_lock);

	spin_lock(&scale_fpriv->client_slock);
	list_del(&client->file_node);
	spin_unlock(&scale_fpriv->client_slock);

	mutex_lock(&scale->se_mutex);
	scale->client_cnt--;
	if (scale->client_cnt == 0) {
		lombo_se_release(scale->se, scale, 1);
		scale->se = NULL;
	}
	mutex_unlock(&scale->se_mutex);

	*handle = 0;
	kfree(client);

	return 0;
}

int lombo_scale_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_scale_file_private *scale_fpriv = file_priv->scale_fpriv;
	struct lombo_scale *scale;
	struct drm_lombo_scale_timeout *timeout = data;
	struct lombo_scale_client *client;

	if (!scale_fpriv) {
		DRM_ERROR("no scale file priv\n");
		return -ENOENT;
	}
	scale = scale_fpriv->scale;

	DRM_DEBUG("%d, handle=%u, time=%llu\n", __LINE__,
		timeout->handle, timeout->time_ns);

	spin_lock(&scale->idr_lock);
	client = idr_find(&scale->client_idr, timeout->handle);
	if (client == NULL) {
		spin_unlock(&scale->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&scale->idr_lock);

	if (timeout->time_ns > 0)
		client->timeout = timeout->time_ns;

	return 0;
}

static int lombo_scale_client_declare_work(
	struct lombo_scale_client *client)
{
	uint32_t cnt = 1;

	while (cnt) {
		spin_lock(&client->wlock);
		if (client->wlocked == 0) {
			client->wlocked = 1;
			spin_unlock(&client->wlock);
			return 0;
		}
		spin_unlock(&client->wlock);
		usleep_range(1, 2);
		DRM_ERROR("scale client%d in diff threads. cnt=%d\n",
			client->id, cnt);
		cnt++;
	}

	return -ETIMEDOUT;
}

static int lombo_scale_client_undeclare_work(
	struct lombo_scale_client *client)
{
	spin_lock(&client->wlock);
	client->wlocked = 0;
	spin_unlock(&client->wlock);

	return 0;
}

static int lombo_scale_client_get_device(
	struct lombo_scale *scale, struct lombo_scale_client *client)
{
	uint64_t time = client->timeout;
	long timeout, ret;
	unsigned long flags;

	spin_lock_irqsave(&scale->work_lock, flags);
	list_add_tail(&client->work_node, &scale->work_list);
	if (scale->state == LOMBO_SCALE_STATE_IDLE) {
		scale->state = LOMBO_SCALE_STATE_BUSSY;
		client->state = LOMBO_SCALE_CLIENT_STATE_PREPARE;
		spin_unlock_irqrestore(&scale->work_lock, flags);
		return 0;
	}
	client->state = LOMBO_SCALE_CLIENT_STATE_WAIT2START;
	spin_unlock_irqrestore(&scale->work_lock, flags);

	time += 999; /* make sure time >= 1000 */
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	ret = wait_event_interruptible_timeout(client->wait,
		scale->state == LOMBO_SCALE_STATE_IDLE, timeout);
	spin_lock_irqsave(&scale->work_lock, flags);
	if (scale->state != LOMBO_SCALE_STATE_IDLE) {
		list_del(&client->work_node);
		client->state = LOMBO_SCALE_CLIENT_STATE_IDLE;
		spin_unlock_irqrestore(&scale->work_lock, flags);
		DRM_ERROR("scale wait2start timeout. timeout=%lu, ret=%lu\n",
			timeout, ret);
		return -ETIMEDOUT;
	}
	scale->state = LOMBO_SCALE_STATE_BUSSY;
	client->state = LOMBO_SCALE_CLIENT_STATE_PREPARE;
	spin_unlock_irqrestore(&scale->work_lock, flags);

	return 0;
}

static int lombo_scale_client_put_device(
	struct lombo_scale *scale,
	struct lombo_scale_client *client)
{
	unsigned long flags;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock_irqsave(&scale->work_lock, flags);
	list_del(&client->work_node);
	scale->state = LOMBO_SCALE_STATE_IDLE;
	if (!list_empty(&scale->work_list)) {
		struct lombo_scale_client *next = list_first_entry(
				&scale->work_list,
				struct lombo_scale_client, work_node);
		wake_up_interruptible(&next->wait);
	}
	spin_unlock_irqrestore(&scale->work_lock, flags);

	return 0;
}

int lombo_scale_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_scale_file_private *scale_fpriv = file_priv->scale_fpriv;
	struct lombo_scale *scale;
	struct drm_lombo_scale_exec_arg *exec_arg = data;

	struct lombo_scale_client *client;
	struct drm_framebuffer *src_fb, *dst_fb;
	struct lombo_se_scale_cfg scale_cfg = { .reqer = NULL, };
	int ret;

	if (!scale_fpriv) {
		DRM_ERROR("no scale file priv\n");
		return -ENOENT;
	}
	scale = scale_fpriv->scale;

	spin_lock(&scale->idr_lock);
	client = idr_find(&scale->client_idr, exec_arg->handle);
	if (client == NULL) {
		spin_unlock(&scale->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&scale->idr_lock);

	src_fb = drm_framebuffer_lookup(drm, exec_arg->in_fb.fb_id);
	dst_fb = drm_framebuffer_lookup(drm, exec_arg->out_fb.fb_id);
	if (!src_fb || !dst_fb) {
		if (src_fb)
			drm_framebuffer_unreference(src_fb);
		else
			DRM_ERROR("not found fb by in_fb_id %d\n",
				  exec_arg->in_fb.fb_id);
		if (dst_fb)
			drm_framebuffer_unreference(dst_fb);
		else
			DRM_ERROR("not found fb by out_fb_id %d\n",
				  exec_arg->out_fb.fb_id);
		return -EINVAL;
	}

	ret = lombo_scale_client_declare_work(client);
	if (ret)
		goto out1;

	scale_cfg.reqer = scale;
	scale_cfg.timeout_ns = client->timeout;
	scale_cfg.src_fb = src_fb;
	scale_cfg.src_cfg = &exec_arg->in_fb;
	scale_cfg.dst_fb = dst_fb;
	scale_cfg.dst_cfg = &exec_arg->out_fb;
	ret = lombo_se_scale_check_fbs(scale->se, &scale_cfg);
	if (ret)
		goto out;

	ret = lombo_scale_client_get_device(scale, client);
	if (ret)
		goto out;

	ret = lombo_se_scale_exec(scale->se, &scale_cfg);

	lombo_scale_client_put_device(scale, client);

out:
	lombo_scale_client_undeclare_work(client);
out1:
	drm_framebuffer_unreference(src_fb);
	drm_framebuffer_unreference(dst_fb);

	return ret;
}

static int lombo_scale_subdrv_open(
	struct drm_device *drm, struct device *dev,
	struct drm_file *file)
{
	struct lombo_scale *scale = dev_get_drvdata(dev);
	struct lombo_scale_file_private *scale_fpriv;
	struct lombo_file_private *file_priv = file->driver_priv;

	DRM_DEBUG("%s,%d\n", __func__, __LINE__);

	scale_fpriv = kzalloc(sizeof(*scale_fpriv), GFP_KERNEL);
	if (!scale_fpriv) {
		DRM_ERROR("%d\n", __LINE__);
		return -ENOMEM;
	}

	scale_fpriv->scale = scale;
	spin_lock_init(&scale_fpriv->client_slock);
	INIT_LIST_HEAD(&scale_fpriv->client_list);

	file_priv->scale_fpriv = scale_fpriv;

	return 0;
}

static void lombo_scale_subdrv_close(
	struct drm_device *drm, struct device *dev,
	struct drm_file *file)
{
	struct lombo_scale *scale = dev_get_drvdata(dev);
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_scale_file_private *scale_fpriv = file_priv->scale_fpriv;
	uint32_t rm_cnt_l = 0;
	uint32_t rm_cnt_i = 0;

	DRM_DEBUG("%s,%d\n", __func__, __LINE__);

	spin_lock(&scale_fpriv->client_slock);
	while (!list_empty(&scale_fpriv->client_list)) {
		struct lombo_scale_client *client_l; /* client in list */
		struct lombo_scale_client *client_i; /* client in idr */

		client_l = list_first_entry(&scale_fpriv->client_list,
			struct lombo_scale_client, file_node);
		list_del(&client_l->file_node);
		spin_unlock(&scale_fpriv->client_slock);
		rm_cnt_l++;

		spin_lock(&scale->idr_lock);
		client_i = idr_find(&scale->client_idr, client_l->id);
		if (client_i == client_l) {
			idr_remove(&scale->client_idr, client_l->id);
			rm_cnt_i++;
		}
		spin_unlock(&scale->idr_lock);

		mutex_lock(&scale->se_mutex);
		if (client_i == client_l)
			scale->client_cnt--;
		if (scale->client_cnt == 0) {
			lombo_se_release(scale->se, scale, 1);
			scale->se = NULL;
		}
		mutex_unlock(&scale->se_mutex);

		kfree(client_l);

		spin_lock(&scale_fpriv->client_slock);
	}
	spin_unlock(&scale_fpriv->client_slock);

	kfree(scale_fpriv);
	file_priv->scale_fpriv = NULL;

	if ((rm_cnt_i > 0) || (rm_cnt_l > 0))
		DRM_ERROR("scale client leak: rm_cnt[%d,%d]\n",
			rm_cnt_l, rm_cnt_i);
}

static int lombo_scale_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct lombo_scale *scale = NULL;
	struct lombo_subdrv *subdrv;
	int ret;

	DRM_DEBUG("%s,%d\n", __func__, __LINE__);

	scale = kzalloc(sizeof(*scale), GFP_KERNEL);
	if (!scale) {
		dev_err(dev, "failed to alloc for scale\n");
		return -ENOMEM;
	}

	mutex_init(&scale->se_mutex);

	idr_init(&scale->client_idr);
	spin_lock_init(&scale->idr_lock);

	spin_lock_init(&scale->work_lock);
	INIT_LIST_HEAD(&scale->work_list);

	dev_set_drvdata(dev, scale);

	subdrv = &scale->subdrv;
	subdrv->dev = dev;
	subdrv->open = lombo_scale_subdrv_open;
	subdrv->close = lombo_scale_subdrv_close;
	ret = lombo_subdrv_register(subdrv);
	if (ret < 0) {
		DRM_ERROR("failed to register scale subdrv.\n");
		kfree(scale);
	}

	return ret;
}

static int lombo_scale_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct lombo_scale *scale = dev_get_drvdata(dev);
	uint32_t client_cnt = scale->client_cnt;
	int ret = 0;

	dev_info(dev, "scale remove\n");

	if (client_cnt > 0)
		dev_err(dev, "remain scale %d client!\n", client_cnt);

	ret = lombo_subdrv_unregister(&scale->subdrv);
	if (ret)
		dev_err(dev, "scale subdrv unregister failed\n");

	dev_set_drvdata(dev, NULL);
	kfree(scale);

	return ret;
}

static struct platform_driver lombo_scale_driver = {
	.driver = {
		.name = "lombo-scale",
		.owner = THIS_MODULE,
		/* .pm = &lombo_scale_pm_ops, */
	},
	.probe = lombo_scale_probe,
	.remove = lombo_scale_remove,
};

int lombo_scale_module_init(void)
{
	struct platform_driver *pdrv = &lombo_scale_driver;
	struct platform_device *pdev;
	int ret = 0;

	pdev = platform_device_register_simple(
			pdrv->driver.name, -1, NULL, 0);
	if (IS_ERR(pdev)) {
		DRM_ERROR("%s: register device fail\n", __func__);
		return (int)PTR_ERR(pdev);
	}

	ret = platform_driver_register(pdrv);
	if (ret) {
		DRM_ERROR("%s: register driver fail, ret=%d\n", __func__, ret);
		platform_device_unregister(pdev);
	}

	return ret;
}
