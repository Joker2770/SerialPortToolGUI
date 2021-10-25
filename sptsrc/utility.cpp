/*************************************************************************
    > File Name: utility.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年09月23日 星期四 10时49分32秒
 ************************************************************************/

/* 
    A Cross-platform serial debug tool.
    Copyright (C) 2021  joker2770(Jintao Yang)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#define INIT 0x0000       /* Initial value */
#define POLINOMIAL 0x1021 /* Polynomial X16+X12+X5+1 */

static unsigned int crc32[256];
static char init = 0;
static void init_table()
{
    int i, j;
    unsigned int crc;
    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
                crc = crc >> 1;
            }
        }
        crc32[i] = crc;
    }
}
//crc32
unsigned int CRC32(unsigned char *buf, unsigned int len)
{
    unsigned int ret = 0xFFFFFFFF;
    int i;
    if (!init)
    {
        init_table();
        init = 1;
    }
    for (i = 0; i < len; i++)
    {
        ret = crc32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
    }
    ret = ~ret;
    return ret;
}

// unsigned int crc16(unsigned char const* PucY, unsigned int UcX)
// {
// 	unsigned short int  uiCrcValue = 0xFFFF;
// 	for (unsigned char ucI = 0; ucI < UcX; ucI++)
// 	{
// 		uiCrcValue = uiCrcValue ^ *(PucY + ucI);
// 		for (unsigned char ucJ = 0; ucJ < 8; ucJ++)
// 		{
// 			if (uiCrcValue & 0x0001)
// 			{
// 				uiCrcValue = (uiCrcValue >> 1) ^ 0x8408;
// 			}
// 			else
// 			{
// 				uiCrcValue = (uiCrcValue >> 1);
// 			}
// 		}
// 	}
// 	return uiCrcValue;
// }

string trimString(string res)
{
    res.erase(remove(res.begin(), res.end(), ' '), res.end());
    res.erase(remove(res.begin(), res.end(), '\t'), res.end());
    res.erase(remove(res.begin(), res.end(), '\r'), res.end());
    res.erase(remove(res.begin(), res.end(), '\n'), res.end());
    return res;
}

string insert_space_split_2(const char *src)
{
    string sDes;
    char szTmp[4] = "\0";
    memset(szTmp, 0, sizeof(szTmp));

    //printf("src: %s\n", src);
    for (int i = 0; i < strlen(src); i++)
    {
        sprintf(szTmp, "%c", src[i]);
        sDes += szTmp;
        if ((i % 2) == 1)
            sDes += " ";
    }
    //printf("src insert space: %s\n", sDes.c_str());

    return sDes;
}

void InvertUint8(unsigned char *dBuf, unsigned char *srcBuf)
{
    int i;
    unsigned char tmp[4];
    tmp[0] = 0;
    for (i = 0; i < 8; i++)
    {
        if (srcBuf[0] & (1 << i))
            tmp[0] |= 1 << (7 - i);
    }
    dBuf[0] = tmp[0];
}

void InvertUint16(unsigned short *dBuf, unsigned short *srcBuf)
{
    int i;
    unsigned short tmp[4];
    tmp[0] = 0;
    for (i = 0; i < 16; i++)
    {
        if (srcBuf[0] & (1 << i))
            tmp[0] |= 1 << (15 - i);
    }
    dBuf[0] = tmp[0];
}

void InvertUint32(unsigned int *dBuf, unsigned int *srcBuf)
{
    int i;
    unsigned int tmp[4];

    tmp[0] = 0;

    for (i = 0; i < 32; i++)
    {
        if (srcBuf[0] & (1 << i))
            tmp[0] |= 1 << (15 - i);
    }
    dBuf[0] = tmp[0];
}

/****************************Info********************************************** 
 * Name:    CRC-4/ITU	x4+x+1 
 * Width:	4
 * Poly:    0x03 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
unsigned char CRC4_ITU(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x03;
    unsigned char wChar = 0;

    while (datalen--)
    {
        wChar = *(data++);
        InvertUint8(&wChar, &wChar);
        wCRCin ^= (wChar);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ (wCPoly << 4);
            else
                wCRCin = wCRCin << 1;
        }
    }
    InvertUint8(&wCRCin, &wCRCin);
    return (wCRCin);
}
/****************************Info********************************************** 
 * Name:    CRC-5/EPC	x5+x3+1 
 * Width:	5
 * Poly:    0x09 
 * Init:    0x09 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
unsigned char CRC5_EPC(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x09 << 3;
    unsigned char wCPoly = 0x09 << 3;

    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ (wCPoly);
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin >> 3);
}
/****************************Info********************************************** 
 * Name:    CRC-5/USB	x5+x2+1 
 * Width:	5
 * Poly:    0x05 
 * Init:    0x1F 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x1F 
 * Note: 
 *****************************************************************************/
#if 0
unsigned char CRC5_USB(unsigned char *data, unsigned int datalen)
{
	unsigned char wCRCin = 0x1F<<3;
	unsigned char wCPoly = 0x05;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x80)
				wCRCin = (wCRCin << 1) ^ (wCPoly << 3);
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint8(&wCRCin,&wCRCin);
	return (wCRCin^0x1F);
}
#else
unsigned char CRC5_USB(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x1F;
    unsigned char wCPoly = 0x05;

    InvertUint8(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ (wCPoly >> 3);
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin ^ 0x1F);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-5/ITU	x5+x4+x2+1  
 * Width:	5
 * Poly:    0x15 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
#if 0
unsigned char CRC5_ITU(unsigned char *data, unsigned int datalen)
{
	unsigned char wCRCin = 0x00;
	unsigned char wCPoly = 0x15;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x80)
				wCRCin = (wCRCin << 1) ^ (wCPoly << 3);
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint8(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned char CRC5_ITU(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x15;

    InvertUint8(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ (wCPoly >> 3);
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-6/ITU	x6+x+1 
 * Width:	6
 * Poly:    0x03 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
unsigned char CRC6_ITU(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x03;
    unsigned char wChar = 0;

    while (datalen--)
    {
        wChar = *(data++);
        InvertUint8(&wChar, &wChar);
        wCRCin ^= (wChar);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ (wCPoly << 2);
            else
                wCRCin = wCRCin << 1;
        }
    }
    InvertUint8(&wCRCin, &wCRCin);
    return (wCRCin);
}
/****************************Info********************************************** 
 * Name:    CRC-7/MMC           x7+x3+1  
 * Width:	7
 * Poly:    0x09 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Use:     MultiMediaCard,SD,ect. 
 *****************************************************************************/
unsigned char CRC7_MMC(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x09;

    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ (wCPoly << 1);
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin >> 1);
}
/****************************Info********************************************** 
 * Name:    CRC-8               x8+x2+x+1 
 * Width:	8 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
unsigned char CRC8(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x07;

    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}
/****************************Info********************************************** 
 * Name:    CRC-8/ITU           x8+x2+x+1 
 * Width:	8 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x55 
 * Alias:   CRC-8/ATM 
 *****************************************************************************/
unsigned char CRC8_ITU(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x07;

    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin ^ 0x55);
}
/****************************Info********************************************** 
 * Name:    CRC-8/ROHC          x8+x2+x+1 
 * Width:	8 
 * Poly:    0x07 
 * Init:    0xFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/
#if 0
unsigned char CRC8_ROHC(unsigned char *data, unsigned int datalen)
{
	unsigned char wCRCin = 0xFF;
	unsigned char wCPoly = 0x07;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 0);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x80)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint8(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned char CRC8_ROHC(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0xFF;
    unsigned char wCPoly = 0x07;

    InvertUint8(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-8/MAXIM         x8+x5+x4+1 
 * Width:	8 
 * Poly:    0x31 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Alias:   DOW-CRC,CRC-8/IBUTTON 
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20 
 *****************************************************************************/
#if 0
unsigned char CRC8_MAXIM(unsigned char *data, unsigned int datalen)
{
	unsigned char wCRCin = 0x00;
	unsigned char wCPoly = 0x31;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 0);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x80)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint8(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned char CRC8_MAXIM(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00;
    unsigned char wCPoly = 0x31;

    InvertUint8(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif

/****************************Info********************************************** 
 * Name:    CRC-16/CCITT        x16+x12+x5+1 
 * Width:	16
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT 
 *****************************************************************************/
#if 0
unsigned short CRC16_CCITT(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned short CRC16_CCITT(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x1021;
    unsigned char wChar = 0;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1 
 * Width:	16 
 * Poly:    0x1021 
 * Init:    0xFFFF 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Note: 
 *****************************************************************************/
unsigned short CRC16_CCITT_FALSE(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0xFFFF;
    unsigned short wCPoly = 0x1021;

    while (datalen--)
    {
        wCRCin ^= *(data++) << 8;
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}
/****************************Info********************************************** 
 * Name:    CRC-16/XMODEM       x16+x12+x5+1 
 * Width:	16 
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN 
 *****************************************************************************/
unsigned short CRC16_XMODEM(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x1021;

    while (datalen--)
    {
        wCRCin ^= (*(data++) << 8);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}
/****************************Info********************************************** 
 * Name:    CRC-16/X25          x16+x12+x5+1 
 * Width:	16 
 * Poly:    0x1021 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0XFFFF 
 * Note: 
 *****************************************************************************/
#if 0 
unsigned short CRC16_X25(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
 
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
unsigned short CRC16_X25(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0xFFFF;
    unsigned short wCPoly = 0x1021;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin ^ 0xFFFF);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/MODBUS       x16+x15+x2+1 
 * Width:	16 
 * Poly:    0x8005 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Note: 
 *****************************************************************************/
#if 0 
unsigned short CRC16_MODBUS(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned short CRC16_MODBUS(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0xFFFF;
    unsigned short wCPoly = 0x8005;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/IBM          x16+x15+x2+1 
 * Width:	16 
 * Poly:    0x8005 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA 
 *****************************************************************************/
#if 0
unsigned short CRC16_IBM(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
unsigned short CRC16_IBM(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x8005;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/MAXIM        x16+x15+x2+1 
 * Width:	16 
 * Poly:    0x8005 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Note: 
 *****************************************************************************/
#if 0
unsigned short CRC16_MAXIM(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
unsigned short CRC16_MAXIM(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x8005;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin ^ 0xFFFF);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/USB          x16+x15+x2+1 
 * Width:	16 
 * Poly:    0x8005 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Note: 
 *****************************************************************************/
#if 0
unsigned short CRC16_USB(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
unsigned short CRC16_USB(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0xFFFF;
    unsigned short wCPoly = 0x8005;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = wCRCin >> 1;
        }
    }
    return (wCRCin ^ 0xFFFF);
}
#endif
/****************************Info********************************************** 
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1 
 * Width:	16 
 * Poly:    0x3D65 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Use:     M-Bus,ect. 
 *****************************************************************************/
#if 0
unsigned short CRC16_DNP(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x3D65;
	unsigned char wChar = 0;
	
	while (datalen--) 	
	{
		wChar = *(data++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF) ;
}
#else
unsigned short CRC16_DNP(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x3D65;

    InvertUint16(&wCPoly, &wCPoly);
    while (datalen--)
    {
        wCRCin ^= *(data++);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x01)
                wCRCin = (wCRCin >> 1) ^ wCPoly;
            else
                wCRCin = (wCRCin >> 1);
        }
    }
    return (wCRCin ^ 0xFFFF);
}
#endif

// /****************************Info**********************************************
//  * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
//  * Width:	32
//  * Poly:    0x4C11DB7
//  * Init:    0xFFFFFFF
//  * Refin:   True
//  * Refout:  True
//  * Xorout:  0xFFFFFFF
//  * Alias:   CRC_32/ADCCP
//  * Use:     WinRAR,ect.
//  *****************************************************************************/
// #if 1
// unsigned int CRC32(unsigned char *data, unsigned int datalen)
// {
// 	unsigned int wCRCin = 0xFFFFFFFF;
// 	unsigned int wCPoly = 0x04C11DB7;
// 	unsigned int wChar = 0;
// 	while (datalen--)
// 	{
// 		wChar = *(data++);
// 		InvertUint8((unsigned char *)&wChar,(unsigned char *)&wChar);
// 		wCRCin ^= (wChar << 24);
// 		for(int i = 0;i < 8;i++)
// 		{
// 			if(wCRCin & 0x80000000)
// 				wCRCin = (wCRCin << 1) ^ wCPoly;
// 			else
// 				wCRCin = wCRCin << 1;
// 		}
// 	}
// 	InvertUint32(&wCRCin,&wCRCin);
// 	return (wCRCin ^ 0xFFFFFFFF) ;
// }
// #else
// unsigned int CRC32(unsigned char *data, unsigned int datalen)
// {

// 	unsigned int wCRCin = 0xFFFFFFFF;
// 	unsigned int wCPoly = 0x04C11DB7;

// 	InvertUint32(&wCPoly,&wCPoly);
// 	while (datalen--)
// 	{
// 		wCRCin ^= *(data++);
// 		for(int i = 0;i < 8;i++)
// 		{
// 			if(wCRCin & 0x01)
// 				wCRCin = (wCRCin >> 1) ^ wCPoly;
// 			else
// 				wCRCin = wCRCin >> 1;
// 		}
// 	}
// 	return (wCRCin ^ 0xFFFFFFFF) ;
// }
// #endif

/****************************Info********************************************** 
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1 
 * Width:	32 
 * Poly:    0x4C11DB7 
 * Init:    0xFFFFFFF 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000000 
 * Note: 
 *****************************************************************************/
unsigned int CRC32_MPEG(unsigned char *data, unsigned int datalen)
{

    unsigned int wCRCin = 0xFFFFFFFF;
    unsigned int wCPoly = 0x04C11DB7;
    unsigned int wChar = 0;
    while (datalen--)
    {
        wChar = *(data++);
        wCRCin ^= (wChar << 24);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80000000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}
