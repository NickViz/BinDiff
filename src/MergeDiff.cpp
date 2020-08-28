////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// MergeDiff.cpp : Diff file merging function
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
#include "BufRead.h"
#include "BufWrite.h"
#include "CryptoApiMD5.h"

#pragma pack(push, 1)								// Set proper (dense) structure packing
struct SDiff
{
	unsigned __int64 pos_;
	char data_[BLOCK_SIZE];
};
#pragma pack(pop)


int MergeDiff(LPCTSTR szInFile, LPCTSTR szDiffFile, LPCTSTR szOutFile, LPCTSTR szMD5File)
{
	try
	{
		CBufRead  inbuf(szInFile);
		CBufRead  diff(szDiffFile, sizeof SDiff, false);
		CBufWrite target(szOutFile);
		CCryptoApiMD5 md5(szMD5File != 0);

		SDiff* pDiff = (SDiff*)diff.GetNextBlock();
		unsigned __int64 pos = 0;

		const char *pBuf;
		while ((pBuf = inbuf.GetNextBlock()) != 0)
		{
			if (pDiff != 0  &&  pDiff->pos_ == pos)	// Block from diff file
			{
				target.add(pDiff->data_, BLOCK_SIZE);
				md5.CryptHashData(pDiff->data_, BLOCK_SIZE); // Calculate MD5 if necessary

				pDiff = (SDiff*)diff.GetNextBlock();
			}
			else
			{
				target.add(pBuf, BLOCK_SIZE);
				md5.CryptHashData(pBuf, BLOCK_SIZE);
			}
			pos += BLOCK_SIZE;
		}

		diff.EnableDisplay();
		while (pDiff != 0)	// Add rest of the diff file
		{
			target.add(pDiff->data_, BLOCK_SIZE);
			md5.CryptHashData(pDiff->data_, BLOCK_SIZE);
			pDiff = (SDiff*)diff.GetNextBlock();
		}

		_tprintf(_T("\r              \r"));

		BYTE aHash[MD5LEN + 1];
		if (szMD5File  &&  md5.GetMD5Hash(aHash))
		{
			BYTE aHashOld[MD5LEN * 2 + 1];
			CBufRead md5File(szMD5File, MD5LEN * 2);
			size_t nRead = md5File.DirectRead((char*)aHashOld, MD5LEN * 2);
			aHashOld[MD5LEN * 2] = 0;
			if (nRead == MD5LEN * 2)
			{
				BYTE aHashNew[MD5LEN * 2 + 1];
				memcpy(aHashNew, ConvertMD5toText(aHash), MD5LEN * 2 + 1);
				if (memcmp(aHashNew, aHashOld, MD5LEN * 2) == 0)
					_tprintf(_T("MD5 matched!\r\n"));
				else
				{
					_tprintf(_T("Error! Wrong MD5. Expected %hs, got %hs\r\n"), aHashOld, aHashNew);
					return 2;
				}
			}
			else
				_tprintf(_T("Bad input MD5 file.\r\n"));
		}

		return 0;
	}
	catch (...)
	{
		return 1;
	}
}
