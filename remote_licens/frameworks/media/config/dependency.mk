RECORDER_ADDITIONAL_DEPENDENCIES := \
        libomx_arec libomx_rotate libomx_aenc libomx_venc libomx_vrec \
        libomx_clock libomx_vrender libomx_splitter libomx_muxer

MEDIAPLAYER_ADDITIONAL_DEPENDENCIES := \
        libomx_srender libomx_adec libomx_vdec libomx_arender libsys_sound \
        libomx_demuxer libomx_rotate libomx_clock libomx_vrender libomx_splitter\
		libomx_sdec

AUDIO_ADDITIONAL_DEPENDENCIES := \
        libomx_arec libomx_aenc libomx_splitter

VIDEO_ADDITIONAL_DEPENDENCIES := \
        libomx_rotate libomx_vrec \
        libomx_vrender libomx_splitter

ifeq ($(TARGET_CHIP), n7v1)
OMX_ADDITIONAL_DEPENDENCIES := 	\
	liblombo_malloc libsw_rot
else
OMX_ADDITIONAL_DEPENDENCIES := 	\
	liblombo_malloc
endif
