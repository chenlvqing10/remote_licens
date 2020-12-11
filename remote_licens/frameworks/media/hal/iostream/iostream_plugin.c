#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "iostream_plugin"

#include "oscl.h"
#include "iostream.h"
#include "iostream_plugin.h"
#include "iostream_file.h"
#include "iostream_external.h"
#include "iostream_priv.h"

typedef struct iostream_plugin_priv {
	void *io_hdl;
	ssize_t (*read)(void *s, uint8_t *buf, size_t size);
	ssize_t (*write)(void *s, uint8_t *buf, size_t size);
	int64_t (*seek)(void *s, int64_t offset, int whence);
	int64_t (*tell)(void *s);   /* current file position */
	int64_t (*fsize)(void *s); /* file size */
	long (*ctrl)(void *s, int cmd, void *arg);
	void (*destroy)(void *s);
} iostream_plugin_priv_t;

static ssize_t iostream_read(void *s, uint8_t *buf, size_t size)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}

	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->read(priv->io_hdl, buf, size);
}

static ssize_t iostream_write(void *s, uint8_t *buf, size_t size)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->write(priv->io_hdl, buf, size);
}

static int64_t iostream_seek(void *s, int64_t offset, int whence)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->seek(priv->io_hdl, offset, whence);
}

static int64_t iostream_tell(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->tell(priv->io_hdl);
}

static int64_t iostream_fsize(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->fsize(priv->io_hdl);
}

static long iostream_fill_data(void *s, void *data, size_t size)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	IOStreamData_t iosData;
	iosData.data = data;
	iosData.size = size;
	return priv->ctrl(priv->io_hdl, IOSTREAM_CMD_FILL_DATA, &iosData);
}

static long iostream_ctrl(void *s, int cmd, void *arg)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	iostream_plugin_priv_t *priv = ((AVIOContext *)s)->handle;
	return priv->ctrl(priv->io_hdl, cmd, arg);
}

static void *create_iostream(void *param, iostream_type_e type)
{
	AVIOContext *ctx;
	iostream_plugin_priv_t *priv = NULL;

	ctx = (AVIOContext *)oscl_malloc(sizeof(AVIOContext));
	if (ctx == NULL) {
		OSCL_LOGE("alloc AVIOContext error!");
		return NULL;
	}
	memset(ctx, 0, sizeof(AVIOContext));
	priv = (iostream_plugin_priv_t *)oscl_malloc(sizeof(iostream_plugin_priv_t));
	if (priv == NULL) {
		OSCL_LOGE("alloc stream priv error!");
		goto ERROR;
	}
	memset(priv, 0, sizeof(iostream_plugin_priv_t));
	ctx->handle = priv;

	switch (type) {
	case STREAM_TYPE_FILE:
		priv->io_hdl = iostream_file_create(param);
		if (priv->io_hdl == NULL) {
			OSCL_LOGE("iostream_file_create error!");
			goto ERROR;
		}
		priv->destroy = iostream_file_destroy;
		priv->read = iostream_file_read;
		priv->write = iostream_file_write;
		priv->seek = iostream_file_seek;
		priv->tell = iostream_file_tell;
		priv->fsize = iostream_file_fsize;
		priv->ctrl = iostream_file_ctrl;
		break;
	case STREAM_TYPE_EXTERNAL: {
		priv->io_hdl = iostream_ext_create(param);
		if (priv->io_hdl == NULL) {
			OSCL_LOGE("iostream_ext_create error!");
			goto ERROR;
		}
		priv->destroy = iostream_ext_destroy;
		priv->read = iostream_ext_read;
		priv->write = iostream_ext_write;
		priv->seek = iostream_ext_seek;
		priv->tell = iostream_ext_tell;
		priv->fsize = iostream_ext_fsize;
		priv->ctrl = iostream_ext_ctrl;
		break;
	}
	default:
		OSCL_LOGE("unsupport stream type %d!", type);
		goto ERROR;
	}

	ctx->read = iostream_read;
	ctx->write = iostream_write;
	ctx->seek = iostream_seek;
	ctx->tell = iostream_tell;
	ctx->fsize = iostream_fsize;
	ctx->ctrl = iostream_ctrl;

	return ctx;

ERROR:
	if (priv != NULL)
		oscl_free(priv);
	oscl_free(ctx);
	return NULL;
}

static void destroy_iostream(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("iostream not create yet!");
		return;
	}

	AVIOContext *ctx = s;
	iostream_plugin_priv_t *priv = ctx->handle;
	priv->destroy(priv->io_hdl);
	oscl_free(priv);
	oscl_free(ctx);
}

static iostream_plugin_t g_iostream_plugin = {
	.create = create_iostream,
	.destroy = destroy_iostream,
	.filldata = iostream_fill_data,
	.ctrl = iostream_ctrl,
};

#if defined(__EOS__)
iostream_plugin_t *iostream_plugin_info(void)
{
	OSCL_LOGI("iostream version: %s", IOSTREAM_VERSION);
	return &g_iostream_plugin;
}
#else
iostream_plugin_t *get_plugin_info(void)
{
	OSCL_LOGI("iostream version: %s", IOSTREAM_VERSION);
	return &g_iostream_plugin;
}
#endif

