////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// MakeHash.cpp : Hash file creation function
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
#include "BufWrite.h"
#include "Manager.h"
#include "CryptoApiMD5.h"

int MakeHash(LPCTSTR szInFile, LPCTSTR szHashFile, LPCTSTR szMD5File)
{
	try
	{
		CBufRead   inbuf(szInFile);
		CHashWrite outbuf(szHashFile);
		CCryptoApiMD5 md5(szMD5File != 0);

#ifndef MULTITHREADED	// Single threaded version
		char* pBuf;
		while ((pBuf = inbuf.GetNextBlock()) != 0)
		{
			// Read input file by blocks, calculate hash, write hash file
			CHash hash = ComputeHash(pBuf, BLOCK_SIZE);
			outbuf.add(hash);
			md5.CryptHashData(pBuf, BLOCK_SIZE); // Calculate MD5 if necessary
		}
#else
		static CManager man;								// Thread manager
		size_t readSequence = 0, writeSequence = 0;			// Assembling positions

		bool bRun = true;
		while (bRun)
		{
			Sleep(0);
			DWORD dwRet = WaitForSingleObject(man.GetMutex(), INFINITE);	// Lock mutex
			switch (dwRet)
			{
			case WAIT_OBJECT_0:
				// Check if we have something to write
				if (man.GetCompletedThread(writeSequence) != -1)
				{											// Got appropriate thread
					for (size_t i = 0; i < man.units(); i++)// Write back ready hashes
						outbuf.add(man.hash(i));
					writeSequence += man.units();			// Update write sequence
					man.freeThread();						// Mark thread as ready for work
				}

				// Get free working thread
				if (man.GetFreeThread() != -1)
				{
					// Get some blocks to compute and give them to working thread
					man.units() = 0;						// Amount of given units initially 0
					char* pBuf;
					while (man.units() < MAX_UNITS  && 		// Get data until has space and not EOF
							(pBuf = inbuf.GetNextBlock()) != 0)
					{
						memcpy(man.block(man.units()),		// Copy block to thread structures
								pBuf, BLOCK_SIZE);
						man.units()++;						// Count blocks
					}
					man.sequence() = readSequence;			// Remember sequence number
					readSequence += man.units();			// Update read sequence
					man.startThread();						// Notify thread that it has something to do
				}

				// Exit check
				if (readSequence == writeSequence  &&  inbuf.Eof())	// Wrote all that read and no more data
					bRun = false;

				ReleaseMutex(man.GetMutex());
				break;

			case WAIT_TIMEOUT:
				continue;

			default:
				_tprintf(_T("Error! WaitForSingleObject failed. Error %d.\n"), GetLastError());
				bRun = false;
				ReleaseMutex(man.GetMutex());
				break;
			} // switch
		}
#endif
		_tprintf(_T("\r              \r"));

		BYTE aHash[MD5LEN];
		if (szMD5File  &&  md5.GetMD5Hash(aHash))
		{
			CBufWrite md5File(szMD5File);
			md5File.add(ConvertMD5toText(aHash), MD5LEN * 2);
		}

		return 0;
	}
	catch (...)
	{
		return 1;
	}
}
