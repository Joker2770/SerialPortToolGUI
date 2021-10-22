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

#define INIT       0x0000    /* Initial value */ 
#define POLINOMIAL 0x1021    /* Polynomial X16+X12+X5+1 */ 

static unsigned int CRC32[256];
static char   init = 0;
static void init_table()
{
    int   i,j;
    unsigned int   crc;
    for(i = 0;i < 256;i++)
    {
		crc = i;
        for(j = 0;j < 8;j++)
        {
            if(crc & 1)
            {
				crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
				crc = crc >> 1;
            }
        }
		CRC32[i] = crc;
    }
}
//crc32
unsigned int crc32(unsigned char *buf, unsigned int len)
{
    unsigned int ret = 0xFFFFFFFF;
    int   i;
    if( !init )
    {
		init_table();
		init = 1;
    }
    for(i = 0; i < len;i++)
    {
		ret = CRC32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
    }
	ret = ~ret;
    return ret;
}

string trimString(string res)
{
    res.erase(remove(res.begin(), res.end(), ' '), res.end());
    res.erase(remove(res.begin(), res.end(), '\t'), res.end());
    res.erase(remove(res.begin(), res.end(), '\r'), res.end());
    res.erase(remove(res.begin(), res.end(), '\n'), res.end());
    return res;
}

string insert_space_split_2(const char * src)
{
    string sDes;
    char szTmp[4] = "\0";
    memset(szTmp, 0, sizeof(szTmp));

    //printf("src: %s\n", src);
    for (int i = 0; i < strlen(src); i++)
    {
        sprintf(szTmp, "%c", src[i]);
        sDes += szTmp;
        if ((i%2) == 1)
            sDes += " ";
    }
    //printf("src insert space: %s\n", sDes.c_str());

    return sDes;
}

unsigned int crc16(unsigned char const* PucY, unsigned int UcX)
{
	unsigned short int  uiCrcValue = 0xFFFF;

	for (unsigned char ucI = 0; ucI < UcX; ucI++)
	{
		uiCrcValue = uiCrcValue ^ *(PucY + ucI);
		for (unsigned char ucJ = 0; ucJ < 8; ucJ++)
		{
			if (uiCrcValue & 0x0001)
			{
				uiCrcValue = (uiCrcValue >> 1) ^ 0x8408;
			}
			else
			{
				uiCrcValue = (uiCrcValue >> 1);
			}
		}
	}

	return uiCrcValue;
}

