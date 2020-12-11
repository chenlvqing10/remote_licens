#ifndef __ASSDEC_H__
#define __ASSDEC_H__
#include "sub_common.h"

typedef struct av_sub_pic_pos {
	OMX_S32 x;
	OMX_S32 y;
	OMX_S32 w;
	OMX_S32 h;
} av_sub_pic_pos_t;

typedef struct av_sub_style {
	OMX_S32 has_style_flag;	/* style valid flag, 0 or 1 */
	OMX_S8 *name;
	av_sub_font_style_e font_style;
	OMX_S32 fontsize;
	OMX_S32 primarycolour;	/* subtitle preferred color, &HBBGGRR*/
	OMX_S32 secondarycolour;/* use in Karaoke */
	OMX_S32 outlinecolour;	/* border colour */
	OMX_S32 backcolour;	/* shadow colour */
	font_bold_flag_e bold;		/* 0 close, -1 open */
	font_italic_flag_e italic;		/* 0 close, -1 open */
	font_underline_flag_e underline;	/* 0 close, -1 open */
	font_strikeout_flag_e strikeout;	/* 0 close, -1 open */
	OMX_F32 scalex;		/* horizontal zoom, default 100% */
	OMX_F32 scaley;		/* vertical zoom, default 100% */
	OMX_S32 spacing;	/* word spacing */
	OMX_S32 angle;		/* rotate */
	OMX_S32 borderstyle;	/* border style.*/
	OMX_F32 outline;	/* border width 1-4*/
	OMX_F32 shadow;		/* shadow distance 0-4*/
	av_sub_alignment_e alignment;	/* alignment */
	OMX_S32 marginl;	/* distance from the left,invalid when left aligned
					value:0 ~ playResX*/
	OMX_S32 marginr;	/* distance from the right,invalid when right aligned
					value:0 ~ playResY */
	OMX_S32 marginv;	/* subtitle height, the distance to the bottom
				   when bottom aligned, to the top when top aligned,
				   invalid when center aligned. value:0 ~ playResY */
	OMX_S32 alphalevel;
	av_sub_encode_type_e encoding;	/* encoding style */
} av_sub_style_t;

typedef struct av_sub_event {
	av_subtitle_type_e type;
	OMX_S32 datasize;
	charset_encode_e charset_type;
	OMX_S32 dialogue_marginl; /* if 0, then use the style marginl */
	OMX_S32 dialogue_marginr; /* if 0, then use the style marginr */
	OMX_S32 dialogue_marginv; /* if 0, then use the style marginv */
	av_sub_effect_type_e sub_effect_type;
	OMX_S32 effectstartx;
	OMX_S32 effectendx;
	OMX_S32 effectstarty;
	OMX_S32 effectendy;
	OMX_S32 effectdelaytime;
	OMX_U8 *pdata;
} av_sub_data_t;

typedef struct av_sub_common {
	OMX_S32 index;
	OMX_S32 resx; /* screen width,default value:384 */
	OMX_S32 resy; /* screen height, default value:288 */
	OMX_S64 pts;
	OMX_S32 flags;
	OMX_S64 start_display_time;
	OMX_S64 end_display_time;
} av_sub_common_t;

typedef struct av_subtitle {
	av_sub_common_t info;
	av_sub_style_t style;
	av_sub_data_t data;
} av_subtitle_t;

#endif
