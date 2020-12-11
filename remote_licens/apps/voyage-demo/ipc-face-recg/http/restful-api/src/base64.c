#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

#define SRC_CHAR_SIZE 3
#define BASE_CHAR_SIZE 4
#define CHAR_SIZE 8
#define BASE_DATA_SIZE 6

#define DEFAULT_CODE "UTF-8"


/********************************************************
   Func Name: base64_encode_value
Date Created: 2018-8-2
 Description: 获取对应编码的值
       Input: value_in：需要编码的字符
      Output: 
      Return: 
     Caution: 
*********************************************************/
char base64_encode_value(char value_in)
{
	static const char *encoding =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    	if (value_in > 63) return '=';
    		return encoding[(int)value_in];
}

/********************************************************
   Func Name: base64_decode_value
Date Created: 2018-8-2
 Description: 获取对应解码的值
       Input: value_in：需要解码的字符
      Output: 
      Return: 
     Caution: 
*********************************************************/
int base64_decode_value(char value_in)
{
    static const char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56
        ,57,58,59,60,61,-1,-1,-1,-2,-1
        ,-1,-1,0,1,2,3,4,5,6,7
        ,8,9,10,11,12,13,14,15,16,17
        ,18,19,20,21,22,23,24,25,-1,-1
        ,-1,-1,-1,-1,26,27,28,29,30,31
        ,32,33,34,35,36,37,38,39,40,41
        ,42,43,44,45,46,47,48,49,50,51};

    static const char decoding_size = sizeof(decoding);
    value_in -= 43;
    if (value_in < 0 || value_in >= decoding_size) return -1;
    return decoding[(int)value_in];
}

/********************************************************
   Func Name: base64_encode
Date Created: 2018-8-3
 Description: base64编码
       Input: plaintext_in：源文件
                 length_in：源文件长度
      Output:     code_out：生成编码文件
                length_out：生成编码文件的长度
      Return: 
     Caution: code_out内存由调用函数释放
*********************************************************/
int base64_encode(const char *plaintext_in, int length_in,
	char **code_out, int *length_out)
{
    int iRet = 0;
    // char * pcCode = NULL;
    // char *pcOut = NULL;
    // int iOutLen = length_in * 2;
    // int iLeftNum = iOutLen;
    if (NULL == plaintext_in || 0 == length_in || NULL == code_out || NULL == length_out)
    {
        return -1;
    }

    iRet = base64_encode_calculate(plaintext_in, length_in, code_out, length_out);
    return iRet;
  
}

/********************************************************
   Func Name: base64_encode_calculate
Date Created: 2018-8-2
 Description: 编码算法
       Input: plaintext_in：源文件
                 length_in：源文件长度
      Output:     code_out：生成编码文件
                length_out：生成编码文件的长度
      Return: 
     Caution: code_out内存由调用函数释放
*********************************************************/
int base64_encode_calculate(const char *plaintext_in, int length_in,
	char **code_out, int *length_out)
{
    int iPadLen = 0;
    int iBaseLen = 0;
    int i = 0;
    char *pcOut = NULL;
    char gPadChar[BASE_CHAR_SIZE] = {0};
    char * pcOutIndex = NULL;

    if (NULL == plaintext_in || 0 == length_in || NULL == code_out || NULL == length_out)
    {
        return -1;
    }

    if (0 != length_in % SRC_CHAR_SIZE)
    {
        iPadLen = SRC_CHAR_SIZE - length_in % SRC_CHAR_SIZE;
    }
    iBaseLen = (length_in + iPadLen)* CHAR_SIZE / BASE_DATA_SIZE + 1;

    pcOut = (char *)malloc(sizeof(char) * iBaseLen);
    if (NULL == pcOut)
    {
        return -2;
    }
    memset(pcOut, 0, sizeof(char) * iBaseLen);
    pcOutIndex = pcOut;

    for (i = 0; i < length_in; i += SRC_CHAR_SIZE)
    {
        if (i == (length_in + iPadLen -3) && 0 != iPadLen)
        {
            if (1 == iPadLen)
            {
                gPadChar[0] = base64_encode_value(*(plaintext_in+i) >> 2 & 0x3f);

                gPadChar[1] = base64_encode_value((*(plaintext_in+i) << 6 >> 2 & 0x30) +
			(*(plaintext_in+i+1) >> 4 & 0xf));

                gPadChar[2] = 
			base64_encode_value((*(plaintext_in+i+1) << 4 >> 2 & 0x3c));

                gPadChar[3] = '=';

            }else if (2 == iPadLen)
            {
                gPadChar[0] = base64_encode_value(*(plaintext_in+i) >> 2 & 0x3f);
                gPadChar[1] = base64_encode_value((*(plaintext_in+i) << 6 >> 2 & 0x30));
                gPadChar[2] = '=';
                gPadChar[3] = '=';
            }
            
        }else
        {
            gPadChar[0] =base64_encode_value(*(plaintext_in+i) >> 2 & 0x3f);
            gPadChar[1] = base64_encode_value((*(plaintext_in+i) << 6 >> 2 & 0x30) +
	    	(*(plaintext_in+i+1) >> 4 & 0xf));
            gPadChar[2] = base64_encode_value((*(plaintext_in+i+1) << 4 >> 2 & 0x3c) +
	    	(*(plaintext_in+i+2) >> 6 & 0x3));
            gPadChar[3] = base64_encode_value(*(plaintext_in+i+2) & 0x3f);
        }
        memcpy(pcOutIndex, gPadChar, BASE_CHAR_SIZE);
        pcOutIndex += BASE_CHAR_SIZE;

        memset(gPadChar, 0, BASE_CHAR_SIZE);
    }

    pcOut[iBaseLen-1] = 0;
    *length_out = iBaseLen;
    *code_out = pcOut;

    return 0;
}

/********************************************************
   Func Name: base64_decode
Date Created: 2018-8-3
 Description: base64解码
       Input:       code_in;编码后的文件
                  length_in：编码后的文件长度
      Output: plaintext_out：源文件
                     outlen：源文件长度
      Return: 
     Caution: plaintext_out内存由调用函数释放
*********************************************************/
int base64_decode(char *code_in, int length_in, char **plaintext_out, int *outlen)
{
    int iRet = base64_decode_calculate(code_in, length_in, plaintext_out, outlen);
    return iRet;
}

/********************************************************
   Func Name: base64_decode_calculate
Date Created: 2018-8-3
 Description: 解码算法
       Input:       code_in;编码后的文件
                  length_in：编码后的文件长度
      Output: plaintext_out：源文件
                     outlen：源文件长度
      Return: 
     Caution: plaintext_out内存由调用函数释放
*********************************************************/
int base64_decode_calculate(char *code_in, int length_in,
	char **plaintext_out, int *outlen)
{
    int i = 0, j = 0;
    int iPadNum = 0;
    char *pcSrc = code_in;
    char * pcIndex = NULL;
    int iSrcLen = 0;
    char *pcOut = NULL;

    if (NULL == code_in || NULL == plaintext_out || NULL == outlen)
    {
        return -1;
    }

    while(1)
    {
        pcIndex = strchr(pcSrc, '=');
        if (NULL == pcIndex)
        {
            break;
        }
        iPadNum++;
        pcIndex += 1;
        pcSrc = pcIndex;

    }
    iSrcLen = length_in/4*3 - iPadNum;

    pcOut = (char *)malloc(sizeof(char)*iSrcLen + 1);
    if (NULL == pcOut)
    {
        return -2;
    }
    memset(pcOut, 0, sizeof(char)*iSrcLen + 1);

    for (i = 0, j = 0; i < length_in; i += 4)
    {
        if ((i == length_in-4) && iPadNum > 0)
        {
            if (1 == iPadNum)
            {
                pcOut[j] = (base64_decode_value(code_in[i]) << 2) +
			(base64_decode_value(code_in[i+1]) << 2 >> 6 & 0x3);
                pcOut[j+1] = (base64_decode_value(code_in[i+1]) << 4) +
			(base64_decode_value(code_in[i+2]) << 2 >> 4 & 0xf);
                j += 2;
            }else if (2 == iPadNum)
            {
                pcOut[j] = (base64_decode_value(code_in[i])<<2) +
			(base64_decode_value(code_in[i+1]) << 2 >> 6 &0x3);
                j ++;
            }
        }else
        {
            pcOut[j] = (base64_decode_value(code_in[i])<<2) +
	    	(base64_decode_value(code_in[i+1]) << 2 >> 6 &0x3);
            pcOut[j+1] = (base64_decode_value(code_in[i+1]) << 4) +
	    	(base64_decode_value(code_in[i+2]) << 2 >> 4 & 0xf);
            pcOut[j+2] = (base64_decode_value(code_in[i+2]) << 6) +
	    	(base64_decode_value(code_in[i+3]) & 0x3f);
            j += 3;
        }
    }

    pcOut[iSrcLen] = '\0';
    *plaintext_out = pcOut;
    *outlen = iSrcLen;

    return 0;
}


