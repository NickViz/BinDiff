////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
//
// Copyright (c) 2010-2015 VorontSOFT
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

#pragma once
#include "Functions.h"
#include "Hash.h"

// Buffered reader

class CBufRead
{
public:
	CBufRead(LPCTSTR szName, size_t block_size = BLOCK_SIZE, bool bDisplay = true);
	~CBufRead(void);

	char* GetNextBlock();
	bool Eof() const { return bEof_; }

	void EnableDisplay() { bDisplay_ = true; }

	size_t DirectRead(char *pBuf, size_t nBuf);

private:
	static const size_t MAX_IO_SIZE = 512 * 1024;

	HANDLE	hFile_;
	LARGE_INTEGER fileSize_;
	size_t  block_size_;

	char	buf_[MAX_IO_SIZE];
	size_t	maxRead_;			// Total amount of data in buf
	char*	pRead_;				// Pointer to the read place

	DWORD	dwDisplayTick_;
	bool	bDisplay_;
	bool	bEof_;
};


class CHashRead : public CBufRead
{
public:
	CHashRead(LPCTSTR szName) : CBufRead(szName, sizeof CHash, false) {}

	CHash GetNextHash();

private:
	char* GetNextBlock();
};
