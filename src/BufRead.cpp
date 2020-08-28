////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// BinRead.cpp : Buffered data read class implementation
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

#include "BufRead.h"


CBufRead::CBufRead(LPCTSTR szName, size_t block_size, bool bDisplay) : block_size_(block_size), 
				maxRead_(0), pRead_(buf_), dwDisplayTick_(0), bDisplay_(bDisplay), bEof_(true)
{
	bool bRet = false;

	hFile_ = CreateFile(szName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile_ != INVALID_HANDLE_VALUE)
	{
		if (GetFileSizeEx(hFile_, &fileSize_)  &&  fileSize_.QuadPart % block_size_ == 0)
		{
			bRet = true;
			bEof_ = false;
		}
		else
		{
			_tprintf(_T("Error! File %s size is incorrect. Error %d.\n"), szName, GetLastError());
			CloseHandle(hFile_);
		}
	}
	else
		_tprintf(_T("Error! File %s does not exist.\n"), szName);

	if (!bRet)
		throw 0;
}


CBufRead::~CBufRead(void)
{
	if (hFile_ != INVALID_HANDLE_VALUE)
		CloseHandle(hFile_);
}


char* CBufRead::GetNextBlock()
{
	if (pRead_ + block_size_ > buf_ + maxRead_)	// We can't fulfil current request 
	{
		// Move unread rest of bytes to the begin of the buf
		size_t nLeft = buf_ + maxRead_ - pRead_;
		memmove(buf_, pRead_, nLeft);

		char *pWrite = buf_ + nLeft;

		DWORD dwRead;
		if (ReadFile(hFile_, pWrite, DWORD(MAX_IO_SIZE - nLeft), &dwRead, 0))
		{
			if (bDisplay_ && GetTickCount() - dwDisplayTick_ >= 1000) // Once per second
			{
				_tprintf(_T("\rLeft %uMB     "), (unsigned)(fileSize_.QuadPart / 1024 / 1024));
				dwDisplayTick_ = GetTickCount();
			}
			fileSize_.QuadPart -= dwRead;
		}
		else
		{
			_tprintf(_T("\nError. Failed to read input file. Error %d.\n"), GetLastError());
			return 0;
		}
		pRead_ = buf_;
		maxRead_ = nLeft + dwRead;
	}

	if (pRead_ + block_size_ <= buf_ + maxRead_)
	{
		char* pRet = pRead_;
		pRead_ += block_size_;
		return pRet;
	}
	else
	{
		bEof_ = true;
		return 0;
	}
}


size_t CBufRead::DirectRead(char *pBuf, size_t nBuf)
{
	DWORD dwRead;
	if (ReadFile(hFile_, pBuf, (DWORD)nBuf, &dwRead, 0))
		return dwRead;
	else
	{
		_tprintf(_T("\nError. Failed to read input file. Error %d.\n"), GetLastError());
		return 0;
	}
}


CHash CHashRead::GetNextHash()
{
	void *pBuf = CBufRead::GetNextBlock();
	if (pBuf)
	{
		CHash hash(pBuf);
		return hash;
	}
	else
	{
		CHash hash;
		return hash;
	}
}
