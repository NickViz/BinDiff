////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// BinDiff.cpp : Main function
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

#include "Functions.h"


int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("BinDiff")
#ifdef _WIN64
		_T( " x64")
#endif
		_T(". (C) 2010-2020 VorontSOFT. Version 1.9\n"));
	
	int nRet = 0;

	if (argc >= 4)
	{
		if (_tcsicmp(argv[1], _T("-makehash")) == 0  ||
			_tcsicmp(argv[1], _T("/makehash")) == 0)
			nRet = MakeHash(argv[2], argv[3], argc == 5 ? argv[4] : 0);
		else 
		if ((_tcsicmp(argv[1], _T("-makediff")) == 0  ||
			 _tcsicmp(argv[1], _T("/makediff")) == 0)  &&
			 argc >= 5)
			nRet = MakeDiff(argv[2], argv[3], argv[4], argc == 6 ? argv[5] : 0);
		else
		if ((_tcsicmp(argv[1], _T("-mergediff")) == 0  ||
			 _tcsicmp(argv[1], _T("/mergediff")) == 0)  &&
			 argc >= 5)
			nRet = MergeDiff(argv[2], argv[3], argv[4], argc == 6 ? argv[5] : 0);
		else
			Help();
	}
	else
		Help();

	return nRet;
}


void Help()
{
	_tprintf(_T("This application is intented to make a diff files for the backup purposes\n\n")
			 _T(" BinDiff -makehash  infile infile.hash [infile.md5]\n")
			 _T(" BinDiff -makediff  infile infile.hash diff_file [infile.md5]\n")
			 _T(" BinDiff -mergediff infile diff_file outfile [infile.md5]\n"));
}
