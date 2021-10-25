/*************************************************************************
    > File Name: utility.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年09月23日 星期四 10时49分19秒
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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include<iostream>
using namespace std;

string trimString(string);
string insert_space_split_2(const char*);
unsigned char CRC4_ITU(unsigned char *data, unsigned int datalen);
unsigned char CRC5_EPC(unsigned char *data, unsigned int datalen);
unsigned char CRC5_USB(unsigned char *data, unsigned int datalen);
unsigned char CRC5_ITU(unsigned char *data, unsigned int datalen);
unsigned char CRC6_ITU(unsigned char *data, unsigned int datalen);
unsigned char CRC7_MMC(unsigned char *data, unsigned int datalen);
unsigned char CRC8(unsigned char *data, unsigned int datalen);
unsigned char CRC8_ITU(unsigned char *data, unsigned int datalen);
unsigned char CRC8_ROHC(unsigned char *data, unsigned int datalen);
unsigned char CRC8_MAXIM(unsigned char *data, unsigned int datalen);
unsigned short CRC16_CCITT(unsigned char *data, unsigned int datalen);
unsigned short CRC16_CCITT_FALSE(unsigned char *data, unsigned int datalen);
unsigned short CRC16_XMODEM(unsigned char *data, unsigned int datalen);
unsigned short CRC16_X25(unsigned char *data, unsigned int datalen);
unsigned short CRC16_MODBUS(unsigned char *data, unsigned int datalen);
unsigned short CRC16_IBM(unsigned char *data, unsigned int datalen);
unsigned short CRC16_MAXIM(unsigned char *data, unsigned int datalen);
unsigned short CRC16_USB(unsigned char *data, unsigned int datalen);
unsigned short CRC16_DNP(unsigned char *data, unsigned int datalen);
unsigned int CRC32(unsigned char *data, unsigned int datalen);
unsigned int CRC32_MPEG(unsigned char *data, unsigned int datalen);

#endif



