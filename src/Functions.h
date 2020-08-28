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
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <wincrypt.h>

#include "Hash.h"

void Help();

int MakeHash(LPCTSTR szInFile, LPCTSTR szHashFile, LPCTSTR szMD5File);
int MakeDiff(LPCTSTR szInFile, LPCTSTR szHashFile, LPCTSTR szDiffFile, LPCTSTR szMD5File);
int MergeDiff(LPCTSTR szInFile, LPCTSTR szDiffFile, LPCTSTR szOutFile, LPCTSTR szMD5File);

const size_t BLOCK_SIZE = 4096;

CHash ComputeHash(const char* pBuf, size_t bufSize);


