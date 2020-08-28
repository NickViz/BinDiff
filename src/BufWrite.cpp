////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// BinWrite.cpp : Buffered data write class implementation
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

#include "BufWrite.h"

CBufWrite::CBufWrite(LPCTSTR szName) : filled_(0)
{
	hFile_ = CreateFile(szName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile_ == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Error! Unable to create %s file.\n"), szName);
		throw 0;
	}
}


CBufWrite::~CBufWrite(void)
{
	if (filled_ > 0)
		flush(true);	// Do not throw exception

	if (hFile_ != INVALID_HANDLE_VALUE)
		CloseHandle(hFile_);
}


void CBufWrite::add(const void *ptr, size_t size)
{
	if (filled_ + size > MAX_IO_SIZE)
		flush();

	memcpy(buf_ + filled_, ptr, size);
	filled_ += (DWORD)size;
}


void CBufWrite::flush(bool bNoThrow)
{
	DWORD dwWritten;

	if (!WriteFile(hFile_, buf_, filled_, &dwWritten, 0)  ||  dwWritten != filled_)
	{
		_tprintf(_T("\nError! Unable to write output file. Error %d.\n"), GetLastError());
		if (!bNoThrow)
			throw 0;
	}

	filled_ = 0;
}
