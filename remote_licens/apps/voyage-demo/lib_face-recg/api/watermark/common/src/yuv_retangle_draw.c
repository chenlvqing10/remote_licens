#include <string.h>
#include "stdio.h"
#define LOG_TAG "YUV_RETANGLE_DRAW"
#include <log/log.h>
#include "yuv_retangle_draw.h"

#define DEFAULT_LINE_THICK 1

typedef struct yuv_set_color {
	int y;
	int u;
	int v;
} yuv_color_t;

typedef struct yuv_unit {
	char *addr_y[4];
	char *addr_u;
	char *addr_v;
} yuv_unit_t;

static int set_yuv_color(int r, int g, int b, yuv_color_t *line_color)
{
	int y, u, v;
	y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
	u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
	v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

	y = y < 16 ? 16 : (y > 255 ? 255 : y);
	u = u < 0 ? 0 : (u > 255 ? 255 : u);
	v = v < 0 ? 0 : (v > 255 ? 255 : v);
	line_color->y = y;
	line_color->u = u;
	line_color->v = v;

	return 0;
}

static int get_yuv420sp_unit_addr(unsigned int x, unsigned int y ,
				  yuv_unit_t *yuv_unit, yuv_draw_t *rtd)
{
	unsigned int width_stride;
	/* unsigned int height_aligned; */
	int start_pixcel_x, start_pixcel_y;

	start_pixcel_x = x / 2 * 2;
	start_pixcel_y = y / 2 * 2;

	width_stride = rtd->width_stride;
	/* height_aligned = rtd->height_aligned; */
	yuv_unit->addr_y[0] = (char *) (rtd->buffer_y
		+ start_pixcel_y * width_stride + start_pixcel_x);
	yuv_unit->addr_y[1] = yuv_unit->addr_y[0] + 1;
	yuv_unit->addr_y[2] = yuv_unit->addr_y[0] + width_stride;
	yuv_unit->addr_y[3] = yuv_unit->addr_y[2] + 1;

	yuv_unit->addr_u = (char *) (rtd->buffer_uv
		+ (start_pixcel_y / 2) * width_stride + start_pixcel_x);
	yuv_unit->addr_v = yuv_unit->addr_u + 1;

	return 0;
}

static int yuv_draw_dot(unsigned int x, unsigned int y,
		yuv_color_t *line_color, yuv_draw_t *rtd)
{
	int yuv_mode, i;
	yuv_unit_t dot;
	yuv_mode = rtd->yuv_mode;

	if (x >= rtd->width_stride || y >= rtd->height_aligned)
		return 0;

	if ((rtd->height_aligned % 2) || (rtd->width_stride % 2))
		if (((y + 1) == rtd->height_aligned)
			|| (x + 1) == rtd->width_stride)
			return 0;

	memset(&dot, 0x0, sizeof(yuv_unit_t));

	switch (yuv_mode) {
	case ENC_YUV420SP:
		get_yuv420sp_unit_addr(x, y, &dot, rtd);
		break;

	default:
		break;
	}

	for (i = 0; i < 4; i++)
		*(dot.addr_y[i]) = line_color->y;

	*(dot.addr_u) = line_color->u;
	*(dot.addr_v) = line_color->v;

	return 0;
}

int yuv_draw_straight_line(yuv_dot_t *start, yuv_dot_t *end,
		yuv_color_t *line_color, yuv_draw_t *rtd)
{
	unsigned int x, y, end_count;

	if (start->x != end->x) {
		y = start->y;
		end_count = end->x + 1;
		for (x = start->x; x < end_count; x++)
			yuv_draw_dot(x, y, line_color, rtd);

	} else if (start->y != end->y) {
		x = start->x;
		end_count = end->y + 1;
		for (y = start->y; y < end_count; y++)
			yuv_draw_dot(x, y, line_color, rtd);

	}

	return 0;
}

int yuv_draw_retangle(yuv_dot_t *start, yuv_dot_t *end,
		yuv_color_t *line_color, yuv_draw_t *rtd)
{
	yuv_dot_t ret_dot[4];

	ret_dot[2].x = ret_dot[0].x = start->x;
	ret_dot[1].y = ret_dot[0].y = start->y;
	ret_dot[1].x = ret_dot[3].x = end->x;
	ret_dot[2].y = ret_dot[3].y = end->y;

	yuv_draw_straight_line(&ret_dot[0], &ret_dot[1], line_color, rtd);
	yuv_draw_straight_line(&ret_dot[0], &ret_dot[2], line_color, rtd);
	yuv_draw_straight_line(&ret_dot[2], &ret_dot[3], line_color, rtd);
	yuv_draw_straight_line(&ret_dot[1], &ret_dot[3], line_color, rtd);

	return 0;
}

int yuv_retangle_draw(yuv_draw_t *rtd)
{
	return yuv_retangle_draw_by_color(rtd, 0xFF0000);
}

int yuv_retangle_draw_by_color(yuv_draw_t *rtd, int rgb_color)
{
	yuv_dot_t start_pos, end_pos;
	yuv_color_t line_color;

	start_pos.x = rtd->retangle.start_pos.x;
	start_pos.y = rtd->retangle.start_pos.y;
	end_pos.x = rtd->retangle.end_pos.x;
	end_pos.y = rtd->retangle.end_pos.y;

	if (rtd->buffer_y == NULL || rtd->buffer_uv == NULL) {
		ALOGE("yuv retangle draw: no input buffer!\n");
		return -1;
	}

	set_yuv_color((rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF,
		rgb_color & 0xFF, &line_color);

	return yuv_draw_retangle(&start_pos, &end_pos, &line_color, rtd);
}
