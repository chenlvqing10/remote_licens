#ifndef NIRAPI_H
#define NIRAPI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ax_nir_dt.h"

typedef struct living_result{
	int living_score;
	int living_confidence;
	int living_color_score[3];
}living_result_t;

typedef struct nir_fn_s{
	living_result_t (*nir_recv_buffer)(void *hdl, void *param, int device_id);
} nir_fn_t;

nir_fn_t *nir_open(int w, int h);
nir_fn_t *nir_open_mask(int w, int h);
nir_fn_t *nir_open_outdoors(int w, int h);

void nir_close(void *hdl);

nir_fn_t *rgb_living_open(int w, int h);
nir_fn_t *rgb_color_living_open(int w, int h);
nir_fn_t *rgb_living_open_new(int w, int h);


void rgb_living_close(void *hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
