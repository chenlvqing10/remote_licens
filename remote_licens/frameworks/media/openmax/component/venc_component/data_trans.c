#define DBG_LEVEL		DBG_WARNING
#define LOG_TAG			"video_encoder_component"

#include "oscl.h"
#include "venc_component.h"
#include "lombo_enc_common.h"
#include "lombo_encplugin.h"

#ifdef __ANDROID__
static long fd_2_phy(int fd)
{
	int fd_util;
	int ret;
	struct util_fd2phy fd2phy;

	fd_util = open(UTIL_DEV, O_RDONLY, 0);
	if (fd_util <= 0) {
		OSCL_LOGE("open util fd error %d !\n", fd_util);
		return 0;
	}
	memset(&fd2phy, 0, sizeof(fd2phy));
	fd2phy.fd = fd;
	ret = ioctl(fd_util, LB_UTIL_FD2PHY, &fd2phy);
	if (ret < 0)
		OSCL_LOGE("fd2phy error\n");
	OSCL_LOGD("phy addr 0x%lx !\n", fd2phy.phy_addr);
	close(fd_util);
	return fd2phy.phy_addr;
}

int anw_buf_to_capbuf(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *inbuf,
	venc_capbuf_t *cap_buf)
{
	int ret = -1;
	if (ctx->inbuf_sum.inbuf_mode == ENC_INMODE_ANW_BUF) {
		struct VideoNativeMetadata *metadata = NULL;
		struct private_handle_t *hnd = NULL;
		metadata = (struct VideoNativeMetadata *)inbuf->pBuffer;
		oscl_param_check_exit(metadata->eType == kMetadataBufferTypeANWBuffer,
			OMX_ErrorBadParameter, NULL);
		hnd = (struct private_handle_t *)metadata->pBuffer->handle;
		oscl_param_check_exit(hnd != NULL, OMX_ErrorBadParameter, NULL);

		cap_buf->src_vir_addr[0] = inbuf->pBuffer;
		cap_buf->src_phy_addr[0] = fd_2_phy(hnd->fd);
		OSCL_LOGE("hnd->fd %d, size:%d, w-h:%d-%d, offset:%d",
			hnd->fd, hnd->size, hnd->width, hnd->height, hnd->offset);
		OSCL_LOGE("format:%x, stride:%d, phyaddr:%lx",
			hnd->format, hnd->stride, cap_buf->src_phy_addr[0]);
		ret = 0;
	}
EXIT:
	return ret;

}
int outbuf_to_muxpack(venc_component_private_t *ctx,
	venc_outbuf_info_t *pbuf_info)
{
	return 0;
}

#else

int anw_buf_to_capbuf(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *inbuf,
	venc_capbuf_t *cap_buf)
{
	return -1;
}

int __outbuf_to_muxpack(venc_component_private_t *ctx,
	venc_outbuf_info_t *pbuf_info,
	muxer_vpacket_t *muxer_vpackt)
{
	int ret = 0;
	venc_packet_t *enc_packet;
	enc_packet = &pbuf_info->enc_packt;

	memset(muxer_vpackt, 0, sizeof(muxer_vpacket_t));
	muxer_vpackt->vir_addr = enc_packet->vir_addr[0];
	muxer_vpackt->size = enc_packet->size[0] + enc_packet->size[1];
	if (enc_packet->size[1]) {
		if (ctx->outbuf_sum.tmpbuf_index) {
			OSCL_LOGE("fatal err! tmpbuffer is used(%d-%d)!",
				ctx->outbuf_sum.tmpbuf_index, pbuf_info->index);
			ret = -1;
			goto EXIT;
		}
		if (ctx->outbuf_sum.tmpbuf_size < muxer_vpackt->size) {
			if (ctx->outbuf_sum.tmpbuf_for_cycle) {
				oscl_free(ctx->outbuf_sum.tmpbuf_for_cycle);
				ctx->outbuf_sum.tmpbuf_for_cycle = NULL;
				ctx->outbuf_sum.tmpbuf_size = 0;
			}
		}
		if (ctx->outbuf_sum.tmpbuf_for_cycle == NULL) {
			ctx->outbuf_sum.tmpbuf_for_cycle =
				oscl_zalloc(muxer_vpackt->size);
			ctx->outbuf_sum.tmpbuf_size = muxer_vpackt->size;
		}
		if (ctx->outbuf_sum.tmpbuf_for_cycle == NULL) {
			OSCL_LOGE("malloc for outbuf_sum buf_for_cycle failed! size:%d",
				muxer_vpackt->size);
			ctx->outbuf_sum.tmpbuf_size = 0;
			ret = -1;
			goto EXIT;
		} else {
			muxer_vpackt->vir_addr = ctx->outbuf_sum.tmpbuf_for_cycle;
			memcpy(muxer_vpackt->vir_addr,
				enc_packet->vir_addr[0], enc_packet->size[0]);
			memcpy(muxer_vpackt->vir_addr + enc_packet->size[0],
				enc_packet->vir_addr[1], enc_packet->size[1]);
			ctx->outbuf_sum.tmpbuf_index = pbuf_info->index;
		}
	}
EXIT:
	if (ret < 0) {
		muxer_vpackt->size = enc_packet->size[0];
		OSCL_LOGE("outbuf to maxpacket failed!");
	}
	return ret;
}

int outbuf_to_muxpack(venc_component_private_t *ctx,
	venc_outbuf_info_t *pbuf_info)
{
	int ret;
	muxer_vpacket_t muxer_vpackt;
	ret = __outbuf_to_muxpack(ctx, pbuf_info, &muxer_vpackt);
	if (ret == 0) {
		pbuf_info->header->nFilledLen = sizeof(muxer_vpacket_t);
		memcpy(pbuf_info->header->pBuffer, &muxer_vpackt,
			sizeof(muxer_vpacket_t));
	} else
		save_raw_stream(ctx->lb_component, OUTPUT_PORT,
			muxer_vpackt.vir_addr, muxer_vpackt.size);
	return ret;
}
#endif


