/*
 * dmx_iostream.c - code for iostream.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "dmx_iostream"

#include <dlfcn.h>
#include <oscl.h>
#include <iostream_plugin.h>
#include <demuxer_plugin.h>


typedef void *(*get_plugin)(void);

typedef struct iostream_priv {
	iostream_plugin_t *plugin;
	void *dl_hdl;
	void *ctx;
} iostream_priv_t;

void *dmx_iostream_create(void *param, iostream_type_e type)
{
	iostream_priv_t *iostream;
	char libname[64] = "libiostream.so";
	get_plugin plugin_func;

	iostream = oscl_zalloc(sizeof(iostream_priv_t));
	if (iostream == NULL) {
		OSCL_LOGE("alloc iostream failed!");
		return NULL;
	}

	OSCL_LOGI("opening lib: %s", libname);
	iostream->dl_hdl = dlopen(libname, RTLD_LAZY);
	if (iostream->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", libname, dlerror());
		goto ERR_EXIT;
	}
	plugin_func = (get_plugin)dlsym(iostream->dl_hdl, "get_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get plugin info error!");
		goto ERR_EXIT;
	}
	iostream->plugin = plugin_func();
	if (iostream->plugin == NULL) {
		OSCL_LOGE("get plugin method error!");
		goto ERR_EXIT;
	}

	iostream->ctx = iostream->plugin->create(param, type);
	if (iostream->ctx == NULL) {
		OSCL_LOGE("create iostream error!");
		goto ERR_EXIT;
	}

	return iostream;

ERR_EXIT:
	if (iostream->ctx != NULL)
		iostream->plugin->destroy(iostream->ctx);
	if (iostream->dl_hdl != NULL)
		dlclose(iostream->dl_hdl);
	oscl_free(iostream);
	return NULL;
}

int dmx_iostream_destroy(void *handle)
{
	iostream_priv_t *iostream = handle;

	if (iostream == NULL) {
		OSCL_LOGE("iostream==NULL!");
		return -1;
	}

	if (iostream->ctx != NULL)
		iostream->plugin->destroy(iostream->ctx);
	if (iostream->dl_hdl != NULL)
		dlclose(iostream->dl_hdl);
	oscl_free(iostream);

	return 0;
}

int dmx_iostream_filldata(void *handle, uint8_t *data, size_t size)
{
	iostream_priv_t *iostream = handle;

	if (iostream == NULL) {
		OSCL_LOGE("iostream==NULL!");
		return -1;
	}

	return iostream->plugin->filldata(iostream->ctx, data, size);
}

void *dmx_iostream_get_context(void *handle)
{
	iostream_priv_t *iostream = handle;

	if (iostream == NULL) {
		OSCL_LOGE("iostream==NULL!");
		return NULL;
	}
	return iostream->ctx;
}

