/*************************************************************************
    > File Name: port_control.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Feb 23 19:22:43 2020
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

#ifndef __PORT_CONTROL_H__
#define __PORT_CONTROL_H__

#include "serial/serial.h"

// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#include <string.h>
#include <stdlib.h>

using namespace std;
typedef vector<string> STRVECTOR;
typedef string::size_type STRPOS;

class my_serial_ctrl
{
public:
	my_serial_ctrl();
	~my_serial_ctrl();

	void enumerate_ports(void);
	int open_port(void);
	int close_port(void);
	void show_port_set(void);
	int show_port_more_set(void);
	int port_set(const char *sArg1, const char *sArg2);
	int send_data(const char *sData, bool bHex = false);
	int receive_data(uint32_t sData, bool bHex = false);

private:
	serial::Serial *m_serial;
};

int AUX_split_str(string strSrc, STRVECTOR &vecDest, char cSep);
int my_stricmp(const char *, const char *);
//void my_sleep(unsigned long milliseconds);

/*
	comment: HexToAscii, e.g.: 0x12 0xAC 0x0D -> "12AC0D"
	para: 
		pSrc(in): source string. 
		pDest(out): destination string. 
		nlen(in): count of characters of source string that you will deal with. 
	caution: none. 
*/
void HexToAscii(unsigned char *pSrc, unsigned char *pDest, unsigned int nLen);

/*
comment: StringToHex, e.g.: "12AC0D" -> 0x12 0xAC 0x0D
para��
	pSrc(in): source string. 
	pDest(out): destination string. 
	uilen(out): count of characters of destination string. 
return: uilen. 
caution: The source string only '0-9 a-z A-Z' include, that is can not include '~%$#.,/][\*...', etc. inside the source string. 
*/
int StringToHex(char *pSrc, unsigned char *pDest, unsigned int *uilen);

#endif // __PORT_CONTROL_H__
