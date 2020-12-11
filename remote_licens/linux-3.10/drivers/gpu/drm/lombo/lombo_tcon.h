/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _LOMBO_TCON_H_
#define _LOMBO_TCON_H_

struct seq_file;
struct lombo_tcon;
struct lombo_dc;

#ifdef CONFIG_DRM_LOMBO_TCON

uint32_t lombo_tcon_id(struct lombo_tcon *tcon);

int lombo_tcon_protect_dbreg(struct lombo_tcon *tcon,
	struct lombo_dc *dc, const bool protect);

int lombo_tcon_mode_fixup(struct lombo_tcon *tcon,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode);
int lombo_tcon_get_out_csfmt(struct lombo_tcon *tcon);
int lombo_tcon_set_in_csfmt(struct lombo_tcon *tcon, int csfmt);

int lombo_tcon_set_state(struct lombo_tcon *tcon,
	struct drm_encoder *enc, int prepare);
int lombo_tcon_set_mode(struct lombo_tcon *tcon,
	const struct drm_encoder *enc,
	const struct drm_display_mode *mode,
	const struct drm_display_mode *adjusted_mode);
int lombo_tcon_commit(struct lombo_tcon *tcon);
int lombo_tcon_disable(struct lombo_tcon *tcon);

int lombo_tcon_kms_show(struct lombo_tcon *tcon,
	struct seq_file *m, void *data);

int lombo_tcon_set_gamma_state(struct lombo_tcon *tcon,
	bool enable);
int lombo_tcon_gamma_set(struct lombo_tcon *tcon,
	u16 *r, u16 *g, u16 *b, uint32_t start, uint32_t size);
#else

static inline uint32_t lombo_tcon_id(struct lombo_tcon *tcon)
{
	return -ENODEV;
}

static inline int lombo_tcon_protect_dbreg(struct lombo_tcon *tcon,
	struct lombo_dc *dc, const bool protect)
{
	return -ENODEV;
}

static inline int lombo_tcon_mode_fixup(struct lombo_tcon *tcon,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	return -ENODEV;
}

static inline int lombo_tcon_get_out_csfmt(struct lombo_tcon *tcon)
{
	return -ENODEV;
}

static inline int lombo_tcon_set_in_csfmt(
	struct lombo_tcon *tcon, int csfmt)
{
	return -ENODEV;
}

static inline int lombo_tcon_set_state(struct lombo_tcon *tcon,
	struct drm_encoder *enc, int prepare)
{
	return -ENODEV;
}

static inline int lombo_tcon_set_mode(struct lombo_tcon *tcon,
	const struct drm_encoder *enc,
	const struct drm_display_mode *mode,
	const struct drm_display_mode *adjusted_mode)
{
	return -ENODEV;
}

static inline int lombo_tcon_commit(struct lombo_tcon *tcon)
{
	return -ENODEV;
}

static inline int lombo_tcon_disable(struct lombo_tcon *tcon)
{
	return -ENODEV;
}

static inline int lombo_tcon_kms_show(struct lombo_tcon *tcon,
	struct seq_file *m, void *data)
{
	return -ENODEV;
}

static inline int lombo_tcon_set_gamma_state(struct lombo_tcon *tcon,
	bool enable)
{
	return -ENODEV;
}

static int lombo_tcon_gamma_set(struct lombo_tcon *tcon,
	u16 *r, u16 *g, u16 *b, uint32_t start, uint32_t size)
{
	return -ENODEV;
}
#endif

#endif /* #ifndef _LOMBO_TCON_H_ */
