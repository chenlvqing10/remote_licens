/*
 * Copyright (C) 2013, NVIDIA Corporation.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/err.h>
#include <linux/module.h>

#include <drm/drm_crtc.h>
#include <drm/drm_panel.h>
#include <drm/drm_mipi_dsi.h>
#include "./lombo/lombo_vo_manage.h"
#include <video/mipi_display.h>

static DEFINE_MUTEX(panel_lock);
static LIST_HEAD(panel_list);

/**
 * DOC: drm panel
 *
 * The DRM panel helpers allow drivers to register panel objects with a
 * central registry and provide functions to retrieve those panels in display
 * drivers.
 */

/**
 * drm_panel_init - initialize a panel
 * @panel: DRM panel
 *
 * Sets up internal fields of the panel so that it can subsequently be added
 * to the registry.
 */
void drm_panel_init(struct drm_panel *panel)
{
	INIT_LIST_HEAD(&panel->list);
}
EXPORT_SYMBOL(drm_panel_init);

/**
 * drm_panel_add - add a panel to the global registry
 * @panel: panel to add
 *
 * Add a panel to the global registry so that it can be looked up by display
 * drivers.
 *
 * Return: 0 on success or a negative error code on failure.
 */
int drm_panel_add(struct drm_panel *panel)
{
	mutex_lock(&panel_lock);
	list_add_tail(&panel->list, &panel_list);
	mutex_unlock(&panel_lock);

	return 0;
}
EXPORT_SYMBOL(drm_panel_add);

/**
 * drm_panel_remove - remove a panel from the global registry
 * @panel: DRM panel
 *
 * Removes a panel from the global registry.
 */
void drm_panel_remove(struct drm_panel *panel)
{
	mutex_lock(&panel_lock);
	list_del_init(&panel->list);
	mutex_unlock(&panel_lock);
}
EXPORT_SYMBOL(drm_panel_remove);

/**
 * drm_panel_attach - attach a panel to a connector
 * @panel: DRM panel
 * @connector: DRM connector
 *
 * After obtaining a pointer to a DRM panel a display driver calls this
 * function to attach a panel to a connector.
 *
 * An error is returned if the panel is already attached to another connector.
 *
 * When unloading, the driver should detach from the panel by calling
 * drm_panel_detach().
 *
 * Return: 0 on success or a negative error code on failure.
 */
int drm_panel_attach(struct drm_panel *panel, struct drm_connector *connector)
{
	if (panel->connector)
		return -EBUSY;

	panel->connector = connector;
	panel->drm = connector->dev;

	return 0;
}
EXPORT_SYMBOL(drm_panel_attach);

/**
 * drm_panel_detach - detach a panel from a connector
 * @panel: DRM panel
 *
 * Detaches a panel from the connector it is attached to. If a panel is not
 * attached to any connector this is effectively a no-op.
 *
 * This function should not be called by the panel device itself. It
 * is only for the drm device that called drm_panel_attach().
 *
 * Return: 0 on success or a negative error code on failure.
 */
int drm_panel_detach(struct drm_panel *panel)
{
	panel->connector = NULL;
	panel->drm = NULL;

	return 0;
}
EXPORT_SYMBOL(drm_panel_detach);

int drm_mipi_dsi_panel_attach(struct drm_panel *panel,
	struct mipi_dsi_host *host)
{
	/* sometime panel not need dsi host ops. */
	if (!panel->dsi)
		return 0;

	if (panel->dsi->host)
		return -EBUSY;

	panel->dsi->host = host;

	return 0;
}
EXPORT_SYMBOL(drm_mipi_dsi_panel_attach);

int drm_mipi_dsi_panel_detach(struct drm_panel *panel)
{
	if (panel->dsi)
		panel->dsi->host = NULL;

	return 0;
}
EXPORT_SYMBOL(drm_mipi_dsi_panel_detach);

static ssize_t mipi_dsi_device_transfer(struct mipi_dsi_device *dsi,
					struct mipi_dsi_msg *msg)
{
	struct mipi_dsi_host *host = dsi->host;
	const struct mipi_dsi_host_ops *ops;

	if (!host)
		return -ENOSYS;

	ops = host->ops;
	if (!ops || !ops->transfer)
		return -ENOSYS;

	if (dsi->mode_flags & MIPI_DSI_MODE_LPM)
		msg->flags |= MIPI_DSI_MSG_USE_LPM;

	return ops->transfer(host, msg);
}

/**
 * mipi_dsi_dcs_write_buffer() - transmit a DCS command with payload
 * @dsi: DSI peripheral device
 * @data: buffer containing data to be transmitted
 * @len: size of transmission buffer
 *
 * This function will automatically choose the right data type depending on
 * the command payload length.
 *
 * Return: The number of bytes successfully transmitted or a negative error
 * code on failure.
 */
ssize_t mipi_dsi_dcs_write_buffer(struct mipi_dsi_device *dsi,
				  const void *data, size_t len)
{
	struct mipi_dsi_msg msg = {
		.channel = dsi->channel,
		.tx_buf = data,
		.tx_len = len
	};

	switch (len) {
	case 0:
		return -EINVAL;

	case 1:
		msg.type = MIPI_DSI_DCS_SHORT_WRITE;
		break;

	case 2:
		msg.type = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
		break;

	default:
		msg.type = MIPI_DSI_DCS_LONG_WRITE;
		break;
	}

	return mipi_dsi_device_transfer(dsi, &msg);
}
EXPORT_SYMBOL(mipi_dsi_dcs_write_buffer);

/**
 * mipi_dsi_dcs_write() - send DCS write command
 * @dsi: DSI peripheral device
 * @cmd: DCS command
 * @data: buffer containing the command payload
 * @len: command payload length
 *
 * This function will automatically choose the right data type depending on
 * the command payload length.
 *
 * Return: The number of bytes successfully transmitted or a negative error
 * code on failure.
 */
ssize_t mipi_dsi_dcs_write(struct mipi_dsi_device *dsi, u8 cmd,
			   const void *data, size_t len)
{
	ssize_t err;
	size_t size;
	u8 *tx;

	if (len > 0) {
		size = 1 + len;

		tx = kmalloc(size, GFP_KERNEL);
		if (!tx)
			return -ENOMEM;

		/* concatenate the DCS command byte and the payload */
		tx[0] = cmd;
		memcpy(&tx[1], data, len);
	} else {
		tx = &cmd;
		size = 1;
	}

	err = mipi_dsi_dcs_write_buffer(dsi, tx, size);

	if (len > 0)
		kfree(tx);

	return err;
}
EXPORT_SYMBOL(mipi_dsi_dcs_write);

/**
 * mipi_dsi_dcs_read() - send DCS read request command
 * @dsi: DSI peripheral device
 * @cmd: DCS command
 * @data: buffer in which to receive data
 * @len: size of receive buffer
 *
 * Return: The number of bytes read or a negative error code on failure.
 */
ssize_t mipi_dsi_dcs_read(struct mipi_dsi_device *dsi, u8 cmd, void *data,
			  size_t len)
{
	struct mipi_dsi_msg msg = {
		.channel = dsi->channel,
		.type = MIPI_DSI_DCS_READ,
		.tx_buf = &cmd,
		.tx_len = 1,
		.rx_buf = data,
		.rx_len = len
	};

	return mipi_dsi_device_transfer(dsi, &msg);
}
EXPORT_SYMBOL(mipi_dsi_dcs_read);

int lombo_vo_dev_panel_attach(struct drm_panel *panel,
	struct lombo_vo_host *host)
{
	/* sometime panel not need lombo_vo_host ops. */
	if (!panel->lombo_vo_dev)
		return 0;

	if (panel->lombo_vo_dev->host)
		return -EBUSY;

	panel->lombo_vo_dev->host = host;

	return 0;
}
EXPORT_SYMBOL(lombo_vo_dev_panel_attach);

int lombo_vo_dev_panel_detach(struct drm_panel *panel)
{
	if (panel->lombo_vo_dev)
		panel->lombo_vo_dev->host = NULL;

	return 0;
}
EXPORT_SYMBOL(lombo_vo_dev_panel_detach);

static ssize_t lombo_vo_device_transfer(struct lombo_vo_device *vo,
					struct lombo_vo_msg *msg)
{
	struct lombo_vo_host *host = vo->host;
	const struct lombo_vo_host_ops *ops;

	if (!host)
		return -ENOSYS;

	ops = host->ops;
	if (!ops || !ops->transfer)
		return -ENOSYS;

	return ops->transfer(host, msg);
}

ssize_t lombo_vo_write_buffer(struct lombo_vo_device *vo,
					void *data, size_t len)
{
	struct lombo_vo_msg msg = {
		.tx_buf = data,
		.tx_len = len
	};

	if (!vo || !data)
		return -EINVAL;

	switch (len) {
	case 0:
		return -EINVAL;

	case 1:
		msg.type = LOMBO_CPU_IF_MSG_WR_IDX;
		break;

	case 2:
		msg.type = LOMBO_CPU_IF_MSG_WR_REG;
		break;

	default:
		msg.type = LOMBO_CPU_IF_MSG_WR_REG;
		break;
	}

	return lombo_vo_device_transfer(vo, &msg);
}
EXPORT_SYMBOL(lombo_vo_write_buffer);

#ifdef CONFIG_OF
/**
 * of_drm_find_panel - look up a panel using a device tree node
 * @np: device tree node of the panel
 *
 * Searches the set of registered panels for one that matches the given device
 * tree node. If a matching panel is found, return a pointer to it.
 *
 * Return: A pointer to the panel registered for the specified device tree
 * node or an ERR_PTR() if no panel matching the device tree node can be found.
 *
 * Possible error codes returned by this function:
 *
 * - EPROBE_DEFER: the panel device has not been probed yet, and the caller
 *   should retry later
 * - ENODEV: the device is not available (status != "okay" or "ok")
 */
struct drm_panel *of_drm_find_panel(const struct device_node *np)
{
	struct drm_panel *panel;

	if (!of_device_is_available(np))
		return ERR_PTR(-ENODEV);

	mutex_lock(&panel_lock);

	list_for_each_entry(panel, &panel_list, list) {
		if (panel->dev->of_node == np) {
			mutex_unlock(&panel_lock);
			return panel;
		}
	}

	mutex_unlock(&panel_lock);
	return ERR_PTR(-EPROBE_DEFER);
}
EXPORT_SYMBOL(of_drm_find_panel);
#endif

MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_DESCRIPTION("DRM panel infrastructure");
MODULE_LICENSE("GPL and additional rights");
