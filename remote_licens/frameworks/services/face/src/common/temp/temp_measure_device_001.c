/*
 * temp_measure_device_001.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define LOG_TAG	"uart_temp"
#include <log/log.h>

#include "temp_measure_device_001.h"

#define READ_BUFFER_SIZE 4096
#define UART_DEVICE     "/dev/ttySLB3"

static int measure_frames = 10;
static int measure_frame = 0;

int hal_uart_get_temp_data(temp_stats_t *status_temp,int temp_mode);
int sendCMD(char *cmd,char *read_temp_buf);

int temp_device_001_init(void)
{
	return 0;
}

int temp_device_001_deinit(void)
{
	return 0;
}

float temp_device_001_get(void)
{
	temp_stats_t status_temp = {0};

	if (hal_uart_get_temp_data(&status_temp, 1) < 0) {
		ALOGE("hal_uart_get_temp_data error\n");
		return -1.0f;
	}

	measure_frame++;
	if (measure_frame >= measure_frames) {
		measure_frame = 0;
		ALOGD("hal_uart_get_temp_data\n");
		return status_temp.tmax;
	} else
		return 0.0f;
}

int temp_device_001_set_measure_frames(int _measure_frames)
{
	measure_frames = _measure_frames;

	return 0;
}

int temp_device_001_reset_measure_frame()
{
	measure_frame = 0;

	return 0;
}

// 每隔125ms读取一次温度统计值，一定要等够时间读取,建议测量数量大于等于8帧。

/**判断str1是否以str2开头
 * 如果是返回1
 * 不是返回0
 * 出错返回-1
 * */
int is_begin_with(const char * str1,char *str2)
{
  if(str1 == NULL || str2 == NULL)
    return 0;
  //int len1 = strlen(str1);
  //int len2 = strlen(str2);
  //if((len1 < len2) || (len1 == 0 || len2 == 0))
  //  return 0;
  char *p = str2;
  int i = 0;
  while(*p != '\0')
  {
    if(*p != str1[i])
      return 0;
    p++;
    i++;
  }
  return 1;
}

int init_tty(struct termios *oldtio){
	int fd;
	struct termios newtio;

	//-----------打开uart设备文件------------------
	fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		ALOGE("Open %s failed\n", UART_DEVICE);
		return -1;
	} else {
		//ALOGD("Open %s successfully\n", UART_DEVICE);
	}
	fcntl(fd, F_SETFL, 0);
	if (tcgetattr(fd, oldtio) < 0)
	{
		ALOGE("tcgetattr uart error\n");
		return -1;
	}
	//-----------设置操作参数-----------------------
	memset(&newtio, 0, sizeof(newtio));

	//波特率=115200 数据位=8 使能数据接收
	newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD | CSTOPB;
	newtio.c_iflag = IGNPAR;

	tcflush(fd, TCIOFLUSH);//清空输入缓冲区和输出缓冲区
	tcsetattr(fd, TCSANOW, &newtio);//设置新的操作参数

	return fd;
}

int uart_send(int fd, char *buf, int len)
{
    unsigned int totalSent = 0;
    int bytes_written;

    while (len > 0) {
        bytes_written = write(fd, buf+totalSent, len);
        if (bytes_written < 0) {
            tcflush(fd, TCOFLUSH);
            ALOGE("uart_send error!!\n");
            return -1;
        }
        len -= bytes_written;
		//ALOGE("uart_send bytes_written:%d,len:%d!!\n",bytes_written,len);
        totalSent += bytes_written;
    }
    return 0;
}

int uart_recieve(int fd, char *buf, int len)
{
	fd_set fset;
	struct timeval tv;
	int rval = 0;

	FD_ZERO(&fset);
	FD_SET(fd, &fset);
	tv.tv_sec = 0; //秒
	tv.tv_usec = 250 * 1000;

	if ((rval = select(fd+1, &fset, NULL, NULL, &tv)) < 0) {
		ALOGE("recieve select error = %d \n",rval);
		rval = 0;
	} else if (rval) {
		//printf("recieve select = %d \n",rval);
		rval = read(fd, buf, len);
		//ALOGE("recieve read = %d \n",rval);
	} else {
		//ALOGE("recieve NULL = %d \n",rval);
	}
	return rval;
}

static int parse_temp_buffer_token(temp_stats_t *status_temp,char *read_temp_buf){
	char dig_str[8]={0};
	int iTemp = 0, len = 0;
	char *token;
	char temp_buf[READ_BUFFER_SIZE]={0};
	memset(temp_buf, 0, READ_BUFFER_SIZE);
	memcpy(temp_buf, read_temp_buf, READ_BUFFER_SIZE);

	token = strtok(temp_buf, "\r\n");
	//ALOGE("cheshi------------->token: %s\n",token);
	while(token != NULL){
		//ALOGD("%s %d %s \n", __func__, __LINE__, token);

		if(is_begin_with(token,"serial over")){
			//ALOGD("%s %d serial over, 0x%x,0x%x\n", __func__, __LINE__,token[0],token[1]);
		}else if(is_begin_with(token,"AvgTmp=")){
			len = strlen("AvgTmp=");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			dig_str[3] = token[len+3];
			dig_str[4] = token[len+4];
			dig_str[5] = token[len+5];
			iTemp = strtol(dig_str, NULL, 16);
			//ALOGD("%s %d dig_str:%s,AvgTmp:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->tavg = iTemp/10.0;
		} else if(is_begin_with(token,"Max=")){
			len = strlen("Max=");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			dig_str[3] = token[len+3];
			dig_str[4] = token[len+4];
			dig_str[5] = token[len+5];
			iTemp = strtol(dig_str, NULL, 16);
			//ALOGD("%s %d dig_str:%s,Max:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->tmax = iTemp/10.0;
		} else if(is_begin_with(token,"Min=")){
			len = strlen("Min=");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			dig_str[3] = token[len+3];
			dig_str[4] = token[len+4];
			dig_str[5] = token[len+5];
			iTemp = strtol(dig_str, NULL, 16);
			//ALOGD("%s %d dig_str:%s, Min:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->tmin = iTemp/10.0;
		} else if(is_begin_with(token,"Extremum=")){
			len = strlen("Extremum=");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			dig_str[3] = token[len+3];
			dig_str[4] = token[len+4];
			dig_str[5] = token[len+5];
			iTemp = strtol(dig_str, NULL, 16);
			//ALOGD("%s %d dig_str:%s, Extremum:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->ext = iTemp/10.0;
		} else if(is_begin_with(token,"Ambient Temperature=")){
			len = strlen("Ambient Temperature=");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			dig_str[3] = token[len+3];
			dig_str[4] = token[len+4];
			dig_str[5] = token[len+5];
			iTemp = strtol(dig_str, NULL, 16);
			//ALOGD("%s %d dig_str:%s, Ambient:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->amt = iTemp/10.0;
		} else if(is_begin_with(token,"Forehead Temperature:")){
			len = strlen("Forehead Temperature:");
			dig_str[0] = token[len];
			dig_str[1] = token[len+1];
			dig_str[2] = token[len+2];
			iTemp = strtol(dig_str, NULL, 10);
			//ALOGD("%s %d dig_str:%s, Ambient:0x%0x \n", __func__, __LINE__, dig_str, iTemp);
			status_temp->tmax = iTemp/10.0;
		}
		token = strtok(NULL, "\r\n");
	}

	return 0;
}

int hal_uart_get_temp_data(temp_stats_t *status_temp,int temp_mode)
{
	char read_temp_buf[READ_BUFFER_SIZE]={0};

	int fd = -1;

	struct termios oldtio;
	char cmd_buf[32];

	if((fd = init_tty(&oldtio)) < 0)
	{
		ALOGE("init tty error\n");
		return -1;
	}

	/**** BEGIN: read VCMD=TMP ****/
	if(temp_mode==0){
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "VCMD=TMP");
		//sprintf(cmd_buf, "VCMD=AMT");
		if (strlen(cmd_buf) == 5) {
			strcat(cmd_buf, "   ");
		} else if (strlen(cmd_buf) == 6) {
			strcat(cmd_buf, "  ");
		} else if (strlen(cmd_buf) == 7) {
			strcat(cmd_buf, " ");
		}
		if(uart_send(fd, cmd_buf, strlen(cmd_buf)) != 0){
			ALOGE("uart_send buf:%s error\n",cmd_buf);
			return -1;
		}
		usleep(30*1000);
		memset(read_temp_buf, 0, READ_BUFFER_SIZE);
		if(uart_recieve(fd, read_temp_buf, READ_BUFFER_SIZE)<=0)
		{
			ALOGE("read TMP error!\n");
			close(fd);
			return -1;
		}
		parse_temp_buffer_token(status_temp,read_temp_buf);
		/**** BEGIN: read VCMD=TMP ****/
		usleep(10*1000);
		/**** BEGIN: read VCMD=AMT ****/
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "VCMD=AMT");
		if (strlen(cmd_buf) == 5) {
			strcat(cmd_buf, "   ");
		} else if (strlen(cmd_buf) == 6) {
			strcat(cmd_buf, "  ");
		} else if (strlen(cmd_buf) == 7) {
			strcat(cmd_buf, " ");
		}
		if(uart_send(fd, cmd_buf, strlen(cmd_buf)) != 0){
			ALOGE("uart_send buf:%s error\n",cmd_buf);
			return -1;
		}
		usleep(100*1000);
		memset(read_temp_buf, 0, READ_BUFFER_SIZE);
		if(uart_recieve(fd, read_temp_buf, READ_BUFFER_SIZE)<=0)
		{
			ALOGE("read AMT error!\n");
			close(fd);
			return -1;
		}
	}else{
			/**** BEGIN: read VCMD=FHT ****/
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "VCMD=FHT");
		//sprintf(cmd_buf, "VCMD=AMT");
		if (strlen(cmd_buf) == 5) {
			strcat(cmd_buf, "   ");
		} else if (strlen(cmd_buf) == 6) {
			strcat(cmd_buf, "  ");
		} else if (strlen(cmd_buf) == 7) {
			strcat(cmd_buf, " ");
		}
		usleep(60*1000);
		if(uart_send(fd, cmd_buf, strlen(cmd_buf)) != 0){
			ALOGE("uart_send buf:%s error\n",cmd_buf);
			return -1;
		}
		usleep(200*1000);
		memset(read_temp_buf, 0, READ_BUFFER_SIZE);
		if(uart_recieve(fd, read_temp_buf, READ_BUFFER_SIZE)<=0)
		{
			ALOGE("read FHT error!\n");
			close(fd);
			return -1;
		}
		ALOGD("read temp=%s\n",read_temp_buf);
	}
	parse_temp_buffer_token(status_temp,read_temp_buf);
	/**** BEGIN: read VCMD=AMT ****/

	//ALOGD("###hal_uart_get_temp_data status_temp tmax:%f,tmin:%f,tavg:%f,ext:%f,amt:%f\n", status_temp->tmax,status_temp->tmin,status_temp->tavg,status_temp->ext,status_temp->amt);

	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);

	return 0;
}

/*float hal_uart_get_max_temp(){
	temp_stats_t status_temp;
	if(hal_uart_get_temp_data(&status_temp)<0){
		ALOGE("hal_uart_get_temp_data error!\n");
	}
	return 0.0;
}*/

int getTempVersion(char *read_temp_buf){

	return sendCMD("VCMD=VER",read_temp_buf);

	return 0;
}

int sendCMD(char *cmd,char *read_temp_buf)
{
	ALOGE("sendCMD....start!\n");

	int fd = -1;
	struct termios oldtio;
	char cmd_buf[32];
	if((fd = init_tty(&oldtio)) < 0)
	{
		ALOGE("init tty error\n");
		return -1;
	}
	memset(cmd_buf, 0, sizeof(cmd_buf));
	sprintf(cmd_buf, cmd);
	if (strlen(cmd_buf) == 5) {
		strcat(cmd_buf, "   ");
	} else if (strlen(cmd_buf) == 6) {
		strcat(cmd_buf, "  ");
	} else if (strlen(cmd_buf) == 7) {
		strcat(cmd_buf, " ");
	}
	usleep(100*1000);
	if(uart_send(fd, cmd_buf, strlen(cmd_buf)) != 0){
		ALOGE("uart_send buf:%s error\n",cmd_buf);
		return -1;
	}
	usleep(200*1000);
	memset(read_temp_buf, 0, READ_BUFFER_SIZE);
	if(uart_recieve(fd, read_temp_buf, READ_BUFFER_SIZE)<=0)
	{
		ALOGE("read buf error!\n");
		close(fd);
		return -1;
	}
	if(read_temp_buf){
		ALOGE("read buf=%s\n",read_temp_buf);
	}
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);
	ALOGE("sendCMD....end!\n");
	return 0;
}
