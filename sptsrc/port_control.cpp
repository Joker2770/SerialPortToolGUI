/************************************************************************
    > File Name: port_control.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Feb 23 19:22:50 2020
 ************************************************************************/

/* 
    A Cross-platform serial debug tool.
    Copyright (C) 2021-2023  joker2770(Jintao Yang)

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

#include "port_control.h"
#include <time.h>

int AUX_split_str(string strSrc, STRVECTOR &vecDest, char cSep)
{
	//Current 'cSep' position and previous 'cSep' position.
	STRPOS pos = 0;
	STRPOS prev_pos = 0;
	//Search one by one.
	while ((pos = strSrc.find_first_of(cSep, pos)) != string::npos)
	{
		string strTemp = strSrc.substr(prev_pos, pos - prev_pos);
		vecDest.push_back(strTemp);
		prev_pos = ++pos;
	}
	//Even behind 'cSep' is NULL, as a NULL string.
	if (!strSrc.empty())
		vecDest.push_back(&strSrc[prev_pos]);
	//Return the count of the string.
	return vecDest.size();
}

void HexToAscii(unsigned char *pSrc, unsigned char *pDest, unsigned int nLen)
{
	unsigned char Nibble[2];
	unsigned int i, j;
	for (i = 0; i < nLen; i++)
	{
		Nibble[0] = (pSrc[i] & 0xF0) >> 4;
		Nibble[1] = pSrc[i] & 0x0F;
		for (j = 0; j < 2; j++)
		{
			if (Nibble[j] < 10)
			{
				Nibble[j] += 0x30;
			}
			else
			{
				if (Nibble[j] < 16)
					Nibble[j] = Nibble[j] - 10 + 'A';
			}
			*pDest++ = Nibble[j];
		} // for (int j = ...)
	}	  // for (int i = ...)
}

int StringToHex(char *pSrc, unsigned char *cbuf, unsigned int *nlen)
{
	*nlen = 0;

	unsigned char high, low;
	unsigned int idx, ii = 0;
	unsigned int itlen = (unsigned int)(strlen(pSrc) - (strlen(pSrc) % 2));
	for (idx = 0; idx < itlen; idx += 2)
	{
		high = pSrc[idx];
		low = pSrc[idx + 1];

		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;
		else
			return -1;

		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;
		else
			return -1;

		cbuf[ii++] = high << 4 | low;
	}
	*nlen = (unsigned int)(strlen(pSrc) / 2);
	return 0;
}

my_serial_ctrl::my_serial_ctrl() : m_serial(new serial::Serial())
{
	if (nullptr != this->m_serial)
	{
		serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
		this->m_serial->setTimeout(timeout);
	}
}

my_serial_ctrl::~my_serial_ctrl()
{
	if (nullptr != this->m_serial)
	{
		if (this->m_serial->isOpen())
			this->m_serial->close();

		delete this->m_serial;
		this->m_serial = nullptr;
	}
}

// void my_sleep(unsigned long milliseconds) {
// #ifdef _MSC_VER
// 	Sleep(milliseconds); // 100 ms
// #else
// 	usleep(milliseconds * 1000); // 100 ms
// #endif
// }

int my_stricmp(const char *str1, const char *str2)
{
#ifdef _MSC_VER
	return _stricmp(str1, str2); // 100 ms
#else
	return strcasecmp(str1, str2); // 100 ms
#endif
}

void my_serial_ctrl::enumerate_ports()
{
	vector<serial::PortInfo> devices_found = serial::list_ports();

	vector<serial::PortInfo>::iterator iter = devices_found.begin();

	int i = 0;
	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		i++;
		printf("%d. Port - <%s>\n\tDescription: %s\n\tHardware_id: %s\n\n", i, device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
	}
	printf("Total %d ports could be connect. \n", i);
}

int my_serial_ctrl::open_port(char* errMsg)
{
	char szPort[128] = "";
	memset(szPort, 0, sizeof(szPort));
	memcpy(szPort, this->m_serial->getPort().c_str(), 128);
	if (0 == my_stricmp(szPort, ""))
	{
		printf("PORT can not be NULL! \n");
		printf("Please set port number first! (e.g.: SETPORT:COM1)\n");
		if (NULL != errMsg)
		{
			strcpy(errMsg, "PORT can not be NULL! \n");
			strcat(errMsg, "Please set port number first! \n");
		}
		return -1;
	}
	if (!this->m_serial->isOpen())
	{
		printf("[%s] is not open! \n", szPort);

		try
		{
			this->m_serial->open();
			printf("Open ...\n");
		}
		catch (exception &e)
		{
			printf("Unhandled Exception: %s\n", e.what());
			if (NULL != errMsg)
				strncpy(errMsg, e.what(), 256);
		}

		if (!this->m_serial->isOpen())
		{
			printf("[%s] open failed!\n", szPort);
			return -1;
		}
	}
	else
	{
		if (NULL != errMsg)
			strcpy(errMsg, "SerialException Serial port already open!\n");
	}
	printf("[%s] open succeed!\n", szPort);
	return 0;
}

int my_serial_ctrl::close_port(char* errMsg)
{
	if (this->m_serial->isOpen())
	{
		printf("Port is open!\n");

		try
		{
			this->m_serial->close();
			printf("Close ...\n");
		}
		catch (exception &e)
		{
			printf("Unhandled Exception: %s\n", e.what());
			if (NULL != errMsg)
				strncpy(errMsg, e.what(), 256);
		}

		if (this->m_serial->isOpen())
		{
			printf("Close port failed!\n");
			if (NULL != errMsg)
				strcat(errMsg, "Close port failed!\n");
			return -1;
		}
	}
	printf("Close port succeed!\n");
	return 0;
}

void my_serial_ctrl::show_port_set()
{
	printf("---------------------------------------------\n");
	printf("\tPORT: %s\n\tBAUDRATE: %u\n\tTIMEOUT: %u, %u, %u, %u, %u\n\tBYTESIZE: %d\n\tPARITY: %d\n\tSTOPBITS: %d\n\tFLOWCONTROL: %d\n",
		   this->m_serial->getPort().c_str(),
		   this->m_serial->getBaudrate(),
		   //Number of milliseconds between bytes received to timeout on.
		   this->m_serial->getTimeout().inter_byte_timeout,
		   //A constant number of milliseconds to wait after calling read.
		   this->m_serial->getTimeout().read_timeout_constant,
		   //A multiplier against the number of requested bytes to wait after calling read.
		   this->m_serial->getTimeout().read_timeout_multiplier,
		   //A constant number of milliseconds to wait after calling write.
		   this->m_serial->getTimeout().write_timeout_constant,
		   //A multiplier against the number of requested bytes to wait after calling write.
		   this->m_serial->getTimeout().write_timeout_multiplier,
		   this->m_serial->getBytesize(),
		   this->m_serial->getParity(),
		   this->m_serial->getStopbits(),
		   this->m_serial->getFlowcontrol());
	printf("---------------------------------------------\n");
}

int my_serial_ctrl::show_port_more_set()
{
	if (!this->m_serial->isOpen())
	{
		printf("Be sure to open port first!!!\n");
		return -1;
	}
	printf("---------------------------------------------\n");
	printf("\tCTS: %d\n\tDSR: %d\n\tCD: %d\n\tRI: %d\n",

		   true == this->m_serial->getCTS() ? 1 : 0,
		   true == this->m_serial->getDSR() ? 1 : 0,
		   true == this->m_serial->getCD() ? 1 : 0,
		   true == this->m_serial->getRI() ? 1 : 0);
	printf("---------------------------------------------\n");
	return 0;
}

int my_serial_ctrl::port_set(const char *szCommand, const char *szPara)
{
	if (NULL == szCommand || NULL == szPara || 0 == my_stricmp(szCommand, "") || 0 == my_stricmp(szPara, ""))
	{
		return -1;
	}

	try
	{
		if (0 == my_stricmp(szCommand, "SETPORT"))
			this->m_serial->setPort(std::string(szPara));
		else if (0 == my_stricmp(szCommand, "SETBAUDRATE"))
			this->m_serial->setBaudrate(atol(szPara));
		else if (0 == my_stricmp(szCommand, "SETTIMEOUT"))
		{
			STRVECTOR vDes;
			AUX_split_str(string(szPara), vDes, ',');
			if (0 == my_stricmp(szPara, "") || 5 != vDes.size())
			{
				printf("e.g.: SETTIMEOUT:10000,250,0,250,0\n");
				return -1;
			}
			this->m_serial->setTimeout(atol(vDes[0].c_str()), atol(vDes[1].c_str()), atol(vDes[2].c_str()), atol(vDes[3].c_str()), atol(vDes[4].c_str()));
		}
		else if (0 == my_stricmp(szCommand, "SETBYTESIZE"))
		{
			if (5 > atoi(szPara) || 8 < atoi(szPara))
			{
				printf("Parameter error\n");
				return -1;
			}
			this->m_serial->setBytesize((serial::bytesize_t)atoi(szPara));
		}
		else if (0 == my_stricmp(szCommand, "SETPARITY"))
		{
			if (0 > atoi(szPara) || 4 < atoi(szPara))
			{
				printf("Parameter error\n");
				return -1;
			}
			this->m_serial->setParity((serial::parity_t)atoi(szPara));
		}
		else if (0 == my_stricmp(szCommand, "SETSTOPBITS"))
		{
			if (1 > atoi(szPara) || 3 < atoi(szPara))
			{
				printf("Parameter error\n");
				return -1;
			}
			this->m_serial->setStopbits((serial::stopbits_t)atoi(szPara));
		}
		else if (0 == my_stricmp(szCommand, "SETFLOWCONTROL"))
		{
			if (0 > atoi(szPara) || 2 < atoi(szPara))
			{
				printf("Parameter error\n");
				return -1;
			}
			this->m_serial->setFlowcontrol((serial::flowcontrol_t)atoi(szPara));
		}
		else if (0 == my_stricmp(szCommand, "SETRTS"))
		{
			//Must be open port first.
			if (0 == atoi(szPara))
				this->m_serial->setRTS(false);
			else
				this->m_serial->setRTS(true);
		}
		else if (0 == my_stricmp(szCommand, "SETDTR"))
		{
			//Must be open port first.
			if (0 == atoi(szPara))
				this->m_serial->setDTR(false);
			else
				this->m_serial->setDTR(true);
		}
		else if (0 == my_stricmp(szCommand, "SETBREAK"))
		{
			//Must be open port first.
			if (0 == atoi(szPara))
				this->m_serial->setBreak(false);
			else
				this->m_serial->setBreak(true);
		}
		else
		{
			printf("Unsupported command!\n");
		}
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
	}

	return 0;
}

int my_serial_ctrl::send_data(const char *szData, char* errMsg, bool b_hex)
{
	try
	{
		time_t tt = time(NULL);
		char tTmp[32] = "\0";
		tm* t = localtime(&tt);
		sprintf(tTmp, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

		unsigned char szDest[1024 * 100] = "";
		memset(szDest, 0, sizeof(szDest));
		uint32_t ilen = 0;
		if (b_hex)
		{
			int iret = StringToHex((char *)szData, szDest, &ilen);
			if (0 != iret)
				throw -1;
			size_t bytes_wrote = this->m_serial->write(szDest, ilen);
			char szTmp[1024 * 100] = "";
			memset(szTmp, 0, sizeof(szTmp));
			memcpy(szTmp, szData, bytes_wrote * 2);
			printf("%s ", tTmp);
			printf(">>(hex)%s\n", szTmp);
		}
		else
		{
			size_t bytes_wrote = this->m_serial->write(szData);
			char szTmp[1024 * 100] = "";
			memset(szTmp, 0, sizeof(szTmp));
			memcpy(szTmp, szData, bytes_wrote);
			printf("%s ", tTmp);
			printf(">>(visual)%s\n", szTmp);
		}
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		if (NULL != errMsg)
			strncpy(errMsg, e.what(), 256);
	}
	catch (int erret)
	{
		if (-1 == erret)
		{
			printf("Internal error!\n");
			if (NULL != errMsg)
				strncpy(errMsg, "Internal error!", strlen("Internal error!"));
		}
	}

	return 0;
}

int my_serial_ctrl::receive_data(uint32_t ulength, char* szRecv, char* errMsg, bool b_hex)
{
	try
	{
		if (this->m_serial->available() > 0)
		{
			time_t tt = time(NULL);
			char tTmp[32] = "\0";
			tm *t = localtime(&tt);
			sprintf(tTmp, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

			uint8_t result[1024 * 100] = "";
			memset(result, 0, sizeof(result));
			size_t r_size = this->m_serial->read(result, ulength);

			if (b_hex)
			{
				unsigned char szdest[1024 * 100] = "";
				memset(szdest, 0, sizeof(szdest));
				HexToAscii(result, szdest, r_size);
				printf("%s ", tTmp);
				printf("<<(hex)%s\n", szdest);

				if (szRecv != NULL)
					memcpy(szRecv, szdest, strlen((char *)szdest));
			}
			else
			{
				printf("%s ", tTmp);
				// Assume non-printable character inside.
				printf("<<(visual)");
				for (size_t i = 0; i < r_size; i++)
				{
					printf("%c", result[i]);
				}
				printf("\n");

				if (szRecv != NULL)
					memcpy(szRecv, result, strlen((char *)result));
			}
		}
		else
			throw -2;
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		if (NULL != errMsg)
			strncpy(errMsg, e.what(), 256);
	}
	catch (int &erret)
	{
		return erret;
	}

	return 0;
}

int my_serial_ctrl::wait_2_read_line(uint32_t ulength, char* szRecv, char* errMsg, bool b_hex)
{
	try
	{
		if (this->m_serial->available() > 0)
		{
			time_t tt = time(NULL);
			char tTmp[32] = "\0";
			tm *t = localtime(&tt);
			sprintf(tTmp, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

			uint8_t result[1024 * 100] = "";
			memset(result, 0, sizeof(result));
			string sTmp;
			size_t r_size = this->m_serial->readline(sTmp, ulength);
			if (r_size > 0)
				memcpy(result, sTmp.c_str(), r_size < (1024 * 100) ? r_size : (1024 * 100));
			else
				throw -1;

			if (b_hex)
			{
				unsigned char szdest[1024 * 100] = "";
				memset(szdest, 0, sizeof(szdest));
				HexToAscii(result, szdest, r_size);
				printf("%s ", tTmp);
				printf("<<(hex)%s\n", szdest);

				if (szRecv != NULL)
					memcpy(szRecv, szdest, strlen((char *)szdest));
			}
			else
			{
				printf("%s ", tTmp);
				// Assume non-printable character inside.
				printf("<<(visual)");
				for (size_t i = 0; i < r_size; i++)
				{
					printf("%c", result[i]);
				}
				printf("\n");

				if (szRecv != NULL)
					memcpy(szRecv, result, strlen((char *)result));
			}
		}
		else
			throw -2;
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		if (NULL != errMsg)
			strncpy(errMsg, e.what(), 256);
	}
	catch (int &erret)
	{
		return erret;
	}

	return 0;
}