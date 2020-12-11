/*
 * person_output.h - Writing personnel registration information to Excel.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _PERSON_OUTPUT_H
#define _PERSON_OUTPUT_H

#define PERSON_OUTPUT_FILE_PATH "/mnt/usb/REG/"
#define PERSON_OUTPUT_EXCEL_FILE PERSON_OUTPUT_FILE_PATH"person.xls"
#define PERSON_OUTPUT_EXCEL_SHEET "PERSON OUTPUT SHEET"
#define PERSON_OUTPUT_DCIM_PATH PERSON_OUTPUT_FILE_PATH"DCIM/"

typedef enum
{
    EN_OUTPUT_IDLE,  /*0*/
    EN_OUTPUT_WORKING, /*1*/
    EN_OUTPUT_OK, /*2*/
    EN_OUTPUT_ERROR, /*3*/
    EN_OUTPUT_MAX
} EN_OUTPUT_TYPE;

typedef void (*output_cb)(void *win, EN_OUTPUT_TYPE state, int step);

/**
 * person_output_stop - Stop to output person info
 * None
 *
 * Returns None.
 */
void person_output_stop(output_cb cb);

/**
 * person_output_start - Start to output person info in thread
 * None
 *
 * Returns None.
 */
void person_output_start(void *win, output_cb cb);

#endif /* _PERSON_OUTPUT_H */

