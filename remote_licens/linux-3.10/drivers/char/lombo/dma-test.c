#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/interrupt.h>

#define TRANSFER_COUNTS_1k	1024
#define STEP_IK			1
/* this is test the case that len is not aligned with width */
#define TRANSFER_COUNTS_1026B	1026
#define STEP_1026B		2

#define TEST_COUNT_FOR_EACH_CASE	10000

struct dma_test_data {
	struct device *dev;
	struct dma_chan *chan;
	dma_addr_t dma_src;
	dma_addr_t dma_dst;
	char *src;
	char *dst;
};

struct lombo_dma_data {
	int slave_id;
	int priority;
};

struct lombo_dma_chan_cfg {
	u32 direction;
	u32 src_addr;
	u32 dst_addr;
	u32 src_width;
	u32 dst_width;
	u32 src_burst;
	u32 dst_burst;
};

struct lombo_dma_chan {
	int				idx;
	struct device			*dev;
	struct dma_chan			chan;
	struct dma_async_tx_descriptor	desc;
	struct lombo_dma_chan_cfg	cfg;
	void __iomem			*base;

	/* channel's basic info */
	struct tasklet_struct		tasklet;
	int				slave_id;
	int				priority;
	unsigned long			int_type;
	unsigned long			trans_type;

	/* list for desc */
	spinlock_t			desc_lock;
	struct list_head		chain_pending;
	struct list_head		chain_inprogress;
	bool				idle;

	struct dma_pool			*desc_pool;
};

int start_next_case;
unsigned long random_0;
unsigned char random_1;
struct dma_slave_config dma_config;
struct dma_test_data *dma_data;
static DECLARE_COMPLETION(callback_complete_wait);

/*------------------------------test cases design-----------------------------*/
/**
 * 1. main thread: submit+pending, callback: submit+pending
 * 2. main thread: (submit+submit+pending), callback: (submit+pending)
 */
static int main_thread_case1(void);
static int main_thread_case2(void);

static bool lombo_filter_fn(struct dma_chan *chan, void *fn_param)
{
	struct lombo_dma_chan *echan =
		container_of(chan, struct lombo_dma_chan, chan);
	struct lombo_dma_data *data = fn_param;

	echan->slave_id = data->slave_id;
	echan->priority = data->priority;

	dev_info(echan->dev, "request channel-%d slave id %d, priority %d\n",
		echan->idx, echan->slave_id, echan->priority);
	return true;
}

static void start_next_test(void)
{
	start_next_case = 0;
}

static void callback_func1(void *pparam)
{
	struct dma_async_tx_descriptor *tx;
	dma_cookie_t cookie;
	struct dma_device *dev;
	static int count;
	static dma_cookie_t precookie = DMA_MIN_COOKIE;

	if (start_next_case == 1) {
		pr_err(" %s enter the entry\n", __func__);
		return;
	}
	++count;
	if (count == TEST_COUNT_FOR_EACH_CASE) {
		count = 0;
		start_next_case = 1;
		precookie = DMA_MIN_COOKIE;
		complete(&callback_complete_wait);
		return;
	}

	if ((dma_cookie_t)pparam == precookie + STEP_1026B) {
		/**
		 * adjust whether the number at random_0 in dst is equal
		 * to the number at ramdom_0 in src.
		 */
		if (random_1 == *(dma_data->dst + random_0) &&
			*(dma_data->dst + random_0) ==
					*(dma_data->src + random_0)) {
			/* successed */;
		} else {
			pr_err("dma test error, src:%d, dst:%d, random0: %d, ramdom1:%d, cookie: %d\n",
					*(dma_data->src + random_0),
					*(dma_data->dst + random_0),
					random_0, random_1, precookie + 1);
			BUG();
			return;
		}
	} else {
		pr_err("dma test error, pre:%d, now:%d\n",
						precookie,
						(dma_cookie_t)pparam);
		BUG();
		return;
	}

	precookie = (dma_cookie_t)pparam;

	/**
	 * here we should generate two random numbers, one represents
	 * the place in dst memory, the other is the number
	 * that will be set into this place.
	 */
	get_random_bytes(&random_0, sizeof(unsigned long));
	random_0 = random_0 % TRANSFER_COUNTS_1026B;
	get_random_bytes(&random_1, sizeof(unsigned char));
	*(dma_data->src + random_0) = random_1;

	/* get tx and submit */
	dev = dma_data->chan->device;
	if (dev->device_prep_dma_memcpy) {

		tx = dev->device_prep_dma_memcpy(dma_data->chan,
						 dma_data->dma_dst,
						 dma_data->dma_src,
						 TRANSFER_COUNTS_1026B,
						 DMA_CTRL_ACK);
		if (!tx) {
			pr_err("dma test: fail to prepare dma memcpy\n");
			dma_release_channel(dma_data->chan);
			return;
		}
		tx->callback = callback_func1;
		cookie = tx->tx_submit(tx);

		tx->callback_param = (void *)cookie;

		if (dma_submit_error(cookie)) {
			pr_err("dma test: submit cookie fail\n");
			dma_release_channel(dma_data->chan);
			return;
		}
		/* issue transfer */
		dma_async_issue_pending(dma_data->chan);
	}
}

static int main_thread_case1(void)
{
	struct dma_async_tx_descriptor *tx;
	struct dma_device *dev;
	static dma_cookie_t cookie;
	struct lombo_dma_data data;
	dma_cap_mask_t dma_mask;

	/**
	 * here we should generate two random numbers, one represents
	 * the place in dst memory, the other is the number
	 * that will be set into this place.
	 */
	get_random_bytes(&random_0, sizeof(unsigned long));
	random_0 = random_0 % TRANSFER_COUNTS_1026B;
	get_random_bytes(&random_1, sizeof(unsigned char));
	*(dma_data->src + random_0) = random_1;

	/* get tx and submit */
	dev = dma_data->chan->device;
	if (dev->device_prep_dma_memcpy) {

		tx = dev->device_prep_dma_memcpy(dma_data->chan,
						 dma_data->dma_dst,
						 dma_data->dma_src,
						 TRANSFER_COUNTS_1026B,
						 DMA_CTRL_ACK);
		if (!tx) {
			pr_err("dma test: fail to prepare dma memcpy\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}
		tx->callback = callback_func1;
		cookie = tx->tx_submit(tx);

		tx->callback_param = (void *)cookie;

		if (dma_submit_error(cookie)) {
			pr_err("dma test: submit cookie fail\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}
		/* issue transfer */
		dma_async_issue_pending(dma_data->chan);
	}

	wait_for_completion(&callback_complete_wait);

	/*
	 * for our current test case, we need to make the tasklet
	 * callback in func dmaengine_terminate_all completed as
	 * soon as possible, the below code need to be added.
	 */
	/* disbale the softirq in critical area */
	local_bh_disable();

	dmaengine_terminate_all(dma_data->chan);

	/* release the bh lock, this can make the tasklet callback
	 * be called
	 */
	local_bh_enable();

	/* release this channel */
	dma_release_channel(dma_data->chan);

	/* request this channel again */
	data.slave_id = 0;
	data.priority = 1;

	dma_cap_zero(dma_mask);
	dma_cap_set(DMA_SLAVE, dma_mask);

	dma_data->chan = dma_request_channel(dma_mask, lombo_filter_fn, &data);
	if (!dma_data->chan) {
		pr_err("dma test: request dmatest-rx channel fail\n");
		return 0;
	}

	msleep(2000);

	return 0;
}

static void callback_func2(void *pparam)
{
	struct dma_async_tx_descriptor *tx;
	struct dma_device *dev;
	static dma_cookie_t cookie;
	static int count;
	static dma_cookie_t precookie = DMA_MIN_COOKIE;

	if (start_next_case == 1) {
		/* we need to wait for all the desc completed */
		precookie = DMA_MIN_COOKIE;
		complete(&callback_complete_wait);
		return;
	}
	++count;
	if (count == TEST_COUNT_FOR_EACH_CASE) {
		count = 0;
		start_next_case = 1;
		return;
	}

	if ((dma_cookie_t)pparam == precookie + STEP_1026B) {
		;
	} else {
		pr_err("dma test error, pre:%d, now:%d\n",
					precookie,
					(dma_cookie_t)pparam);
		BUG();
		return;
	}

	precookie = (dma_cookie_t)pparam;

	/* get tx and submit */
	dev = dma_data->chan->device;
	if (dev->device_prep_dma_memcpy) {

		tx = dev->device_prep_dma_memcpy(dma_data->chan,
						 dma_data->dma_src,
						 dma_data->dma_dst,
						 TRANSFER_COUNTS_1026B,
						 DMA_CTRL_ACK);
		if (!tx) {
			pr_err("dma test: fail to prepare dma memcpy\n");
			dma_release_channel(dma_data->chan);
			return;
		}
		tx->callback = callback_func2;
		cookie = tx->tx_submit(tx);

		tx->callback_param = (void *)cookie;

		if (dma_submit_error(cookie)) {
			pr_err("dma test: submit cookie fail\n");
			dma_release_channel(dma_data->chan);
			return;
		}

		/* issue transfer */
		dma_async_issue_pending(dma_data->chan);
	}
}

static int main_thread_case2(void)
{
	struct dma_async_tx_descriptor *tx;
	struct dma_async_tx_descriptor *tx1;
	struct dma_device *dev;
	static dma_cookie_t cookie;
	static dma_cookie_t cookie1;
	struct lombo_dma_data data;
	dma_cap_mask_t dma_mask;

	/* get tx and submit */
	dev = dma_data->chan->device;
	if (dev->device_prep_dma_memcpy) {

		tx = dev->device_prep_dma_memcpy(dma_data->chan,
						 dma_data->dma_src,
						 dma_data->dma_dst,
						 TRANSFER_COUNTS_1026B,
						 DMA_CTRL_ACK);
		if (!tx) {
			pr_err("dma test: fail to prepare dma memcpy\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}

		tx->callback = callback_func2;
		cookie = tx->tx_submit(tx);
		tx->callback_param = (void *)cookie;

		if (dma_submit_error(cookie)) {
			pr_err("dma test: submit cookie fail\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}

		/* submit the sencond times*/
		tx1 = dev->device_prep_dma_memcpy(dma_data->chan,
						 dma_data->dma_src,
						 dma_data->dma_dst,
						 TRANSFER_COUNTS_1026B,
						 DMA_CTRL_ACK);
		if (!tx1) {
			pr_err("dma test: fail to prepare dma memcpy\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}

		tx1->callback = callback_func2;
		cookie1 = tx1->tx_submit(tx1);
		tx1->callback_param = (void *)cookie1;

		if (dma_submit_error(cookie1)) {
			pr_err("dma test: submit cookie fail\n");
			dma_release_channel(dma_data->chan);
			return 0;
		}
		/* issue transfer */
		dma_async_issue_pending(dma_data->chan);
	}

	wait_for_completion(&callback_complete_wait);

	/*
	 * for our current test case, we need to make the tasklet
	 * callback in func dmaengine_terminate_all completed as
	 * soon as possible, the below code need to be added.
	 */
	/* disbale the softirq in critical area */
	local_bh_disable();

	dmaengine_terminate_all(dma_data->chan);

	/* release the bh lock, this can make the tasklet callback
	 * be called
	 */
	local_bh_enable();

	/* release this channel */
	dma_release_channel(dma_data->chan);

	/* request this channel again */
	data.slave_id = 0;
	data.priority = 1;

	dma_cap_zero(dma_mask);
	dma_cap_set(DMA_SLAVE, dma_mask);

	dma_data->chan = dma_request_channel(dma_mask, lombo_filter_fn, &data);
	if (!dma_data->chan) {
		pr_err("dma test: request dmatest-rx channel fail\n");
		return 0;
	}

	msleep(2000);

	return 0;
}

void dma_test(void)
{
/**
 * 1. main thread: submit+pending, callback: submit+pending
 * 2. main thread: (submit+submit+ pending), callback:(submit+pending)
 */
	while (1) {
		pr_err("\n");
		pr_err("\n");
		INIT_COMPLETION(callback_complete_wait);
		start_next_test();
		pr_err("start test case 1\n");
		main_thread_case1();
		pr_err("test case 1 end\n");

		pr_err("\n");
		INIT_COMPLETION(callback_complete_wait);
		start_next_test();
		pr_err("start test case 2\n");
		main_thread_case2();
		pr_err("test case 2 end\n");
	}
}


static int dma_test_probe(struct platform_device *pdev)
{
	int ret;

	dma_data = devm_kzalloc(&pdev->dev, sizeof(*dma_data), GFP_KERNEL);
	if (!dma_data) {
		dev_err(&pdev->dev, "alloc dma test data fail\n");
		return -ENOMEM;
	}

	dma_data->dev = &pdev->dev;
	dma_data->chan = NULL;
	dma_data->dma_src = 0;
	dma_data->dma_dst = 0;
	dma_data->src = NULL;
	dma_data->dst = NULL;

	dma_data->src = dma_alloc_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
					   &dma_data->dma_src, GFP_KERNEL);
	dma_data->dst = dma_alloc_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
					   &dma_data->dma_dst, GFP_KERNEL);

	if (!dma_data->src || !dma_data->dst) {
		if (dma_data->src) {
			dma_free_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
					dma_data->src, dma_data->dma_src);
			devm_kfree(&pdev->dev, dma_data);
		}
		dev_err(&pdev->dev, "alloc coherent memory failed\n");
		return -ENOMEM;
	}

	/* request dma channel */
	dma_data->chan = dma_request_slave_channel(dma_data->dev, "dmatest-rx");
	if (!dma_data->chan) {
		dev_err(&pdev->dev, "request dmatest-rx channel failed\n");
		ret = -EINVAL;
		goto err_req_chan;
	}

	dma_config.direction = DMA_MEM_TO_MEM;
	dma_config.src_addr = dma_data->dma_src;
	dma_config.dst_addr = dma_data->dma_dst;
	dma_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	dma_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
	dma_config.src_maxburst = 4;
	dma_config.dst_maxburst = 4;

	ret = dmaengine_slave_config(dma_data->chan, &dma_config);
	if (ret < 0) {
		dev_err(&pdev->dev, "slave config falid\n");
		goto err_slave_conf;
	}
	return 0;

err_slave_conf:
	dma_release_channel(dma_data->chan);
err_req_chan:
	dma_free_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
				dma_data->src, dma_data->dma_src);
	dma_free_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
				dma_data->dst, dma_data->dma_dst);
	devm_kfree(&pdev->dev, dma_data);

	return ret;
}

static int dma_test_remove(struct platform_device *pdev)
{
	if (dma_data->chan)
		dma_release_channel(dma_data->chan);
	dma_free_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
				dma_data->src, dma_data->dma_src);
	dma_free_coherent(&pdev->dev, TRANSFER_COUNTS_1026B,
				dma_data->dst, dma_data->dma_dst);
	devm_kfree(&pdev->dev, dma_data);
	return 0;
}

static const struct of_device_id dma_test_match[] = {
	{ .compatible = "dma-test", },
	{},
};
MODULE_DEVICE_TABLE(of, dma_test_match);


static struct platform_driver dma_test_platform_driver = {
	.probe  = dma_test_probe,
	.remove = dma_test_remove,
	.driver = {
		.name  = "dma-test",
		.owner = THIS_MODULE,
		.of_match_table = dma_test_match,
	},
};

static int __init dma_test_init(void)
{
	pr_info("dma test driver register\n");

	return platform_driver_register(&dma_test_platform_driver);
}

static void __exit dma_test_exit(void)
{
	pr_info("dma test driver unregister\n");

	platform_driver_unregister(&dma_test_platform_driver);
}

module_init(dma_test_init);
module_exit(dma_test_exit);

MODULE_DESCRIPTION("LomboTech DMA test Driver");
MODULE_AUTHOR("LomboTech Co.Ltd");
MODULE_LICENSE("GPL");
