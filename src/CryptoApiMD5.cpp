////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// CryptoApiMD5.cpp : MD5 calculation implementation
//
// Copyright (c) 2010-2020 VorontSOFT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "CryptoApiMD5.h"
#include <tchar.h>
#include <stdio.h>

CCryptoApiMD5::CCryptoApiMD5(bool bInit) : m_hProv(0), m_hHash(0), m_bInit(bInit)
{
	if (m_bInit)
	{
		if (!CryptAcquireContext(&m_hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			_tprintf(_T("CryptAcquireContext failed: %d\r\n"), GetLastError()); 
			m_bInit = false;
			return;
		}
		if (!CryptCreateHash(m_hProv, CALG_MD5, 0, 0, &m_hHash))
		{
			_tprintf(_T("CryptCreateHash failed: %d\r\n"), GetLastError()); 
			m_bInit = false;
		}
	}
}


CCryptoApiMD5::~CCryptoApiMD5()
{
	if (m_hHash)
		CryptDestroyHash(m_hHash);

	if (m_hProv)
		CryptReleaseContext(m_hProv, 0);
}


bool CCryptoApiMD5::CryptHashData(const char* pBuf, size_t bufSize)
{
	if (m_bInit)
	{
		if (!::CryptHashData(m_hHash, (const BYTE*)pBuf, (DWORD)bufSize, 0))
        {
			_tprintf(_T("CryptHashData failed: %d\r\n"), GetLastError()); 
			m_bInit = false;
			return false;
        }
	}
	return true;
}


bool CCryptoApiMD5::GetMD5Hash(BYTE bHash[])
{
	DWORD cbHash = MD5LEN;
	return (m_bInit  &&  CryptGetHashParam(m_hHash, HP_HASHVAL, bHash, &cbHash, 0));
}


char *ConvertMD5toText(BYTE bHash[])
{
	static char szOutBuf[2][MD5LEN * 2 + 1];
	static size_t i = 0;
	
	if (++i > 1)
		i = 0;

	for (size_t j = 0; j < MD5LEN; j++)
		_snprintf_s(szOutBuf[i] + j * 2, 3, _TRUNCATE, "%02X", bHash[j]);
	szOutBuf[i][MD5LEN * 2] = 0;
	return szOutBuf[i];
}