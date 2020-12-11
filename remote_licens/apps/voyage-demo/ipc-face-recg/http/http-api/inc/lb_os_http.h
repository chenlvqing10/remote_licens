/*
* gui/lbui/lbal/osal/lb_os_mem.h - header file for memory allocate
* and lb_mem_free porting interface.
*
* Copyright (C) 2016-2018, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* Common code for LomboTech Socs
*
* This program is lb_mem_free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the lb_mem_free Software Foundation.
*
*/
#ifndef _LB_OS_MEM_H_
#define _LB_OS_MEM_H_

/*----------------------------------------------*
 * header files                           *
 *----------------------------------------------*/

#include <stdlib.h>

/*----------------------------------------------*
 * defines                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * structs                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * extern functions                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * functions                                    *
 *----------------------------------------------*/

// #define lb_mem_alloc(s) lb_mem_alloc_ext(s, __func__, __LINE__)

void *lb_mem_alloc_ext(int size, const char *func, int lines);

void lb_mem_dump(void);

void lb_mem_free(void *p);


#endif
