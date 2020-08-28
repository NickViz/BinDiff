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

class CHash
{
public:
	char hash_[24];	// We use 24 bytes length hash to get 2.2e-35 collision probability.

	CHash() { memset(hash_, 0, sizeof hash_); }
	CHash(void *ptr) { memcpy(hash_, ptr, sizeof hash_); }

	CHash(const CHash& other) { memcpy(hash_, other.hash_, sizeof hash_); }
	CHash& operator=(const CHash& other) { memcpy(hash_, other.hash_, sizeof hash_); return *this; }

	bool operator==(const CHash& other) { return memcmp(hash_, other.hash_, sizeof hash_) == 0; }
};
