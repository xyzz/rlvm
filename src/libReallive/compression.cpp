// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of libReallive, a dependency of RLVM. 
// 
// -----------------------------------------------------------------------
//
// Copyright (c) 2006, 2007 Peter Jolly
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -----------------------------------------------------------------------

#include "Precompiled.hpp"

// -----------------------------------------------------------------------

#include "compression.h"

namespace libReallive {
namespace Compression {

/* RealLive uses a rather basic XOR encryption scheme, to which this
 * is the key. */
static char xor_mask[256] = {
  0x8b, 0xe5, 0x5d, 0xc3, 0xa1, 0xe0, 0x30, 0x44, 0x00, 0x85, 0xc0, 0x74, 0x09, 0x5f, 0x5e, 0x33,
  0xc0, 0x5b, 0x8b, 0xe5, 0x5d, 0xc3, 0x8b, 0x45, 0x0c, 0x85, 0xc0, 0x75, 0x14, 0x8b, 0x55, 0xec,
  0x83, 0xc2, 0x20, 0x52, 0x6a, 0x00, 0xe8, 0xf5, 0x28, 0x01, 0x00, 0x83, 0xc4, 0x08, 0x89, 0x45,
  0x0c, 0x8b, 0x45, 0xe4, 0x6a, 0x00, 0x6a, 0x00, 0x50, 0x53, 0xff, 0x15, 0x34, 0xb1, 0x43, 0x00,
  0x8b, 0x45, 0x10, 0x85, 0xc0, 0x74, 0x05, 0x8b, 0x4d, 0xec, 0x89, 0x08, 0x8a, 0x45, 0xf0, 0x84,
  0xc0, 0x75, 0x78, 0xa1, 0xe0, 0x30, 0x44, 0x00, 0x8b, 0x7d, 0xe8, 0x8b, 0x75, 0x0c, 0x85, 0xc0,
  0x75, 0x44, 0x8b, 0x1d, 0xd0, 0xb0, 0x43, 0x00, 0x85, 0xff, 0x76, 0x37, 0x81, 0xff, 0x00, 0x00,
  0x04, 0x00, 0x6a, 0x00, 0x76, 0x43, 0x8b, 0x45, 0xf8, 0x8d, 0x55, 0xfc, 0x52, 0x68, 0x00, 0x00,
  0x04, 0x00, 0x56, 0x50, 0xff, 0x15, 0x2c, 0xb1, 0x43, 0x00, 0x6a, 0x05, 0xff, 0xd3, 0xa1, 0xe0,
  0x30, 0x44, 0x00, 0x81, 0xef, 0x00, 0x00, 0x04, 0x00, 0x81, 0xc6, 0x00, 0x00, 0x04, 0x00, 0x85,
  0xc0, 0x74, 0xc5, 0x8b, 0x5d, 0xf8, 0x53, 0xe8, 0xf4, 0xfb, 0xff, 0xff, 0x8b, 0x45, 0x0c, 0x83,
  0xc4, 0x04, 0x5f, 0x5e, 0x5b, 0x8b, 0xe5, 0x5d, 0xc3, 0x8b, 0x55, 0xf8, 0x8d, 0x4d, 0xfc, 0x51,
  0x57, 0x56, 0x52, 0xff, 0x15, 0x2c, 0xb1, 0x43, 0x00, 0xeb, 0xd8, 0x8b, 0x45, 0xe8, 0x83, 0xc0,
  0x20, 0x50, 0x6a, 0x00, 0xe8, 0x47, 0x28, 0x01, 0x00, 0x8b, 0x7d, 0xe8, 0x89, 0x45, 0xf4, 0x8b,
  0xf0, 0xa1, 0xe0, 0x30, 0x44, 0x00, 0x83, 0xc4, 0x08, 0x85, 0xc0, 0x75, 0x56, 0x8b, 0x1d, 0xd0,
  0xb0, 0x43, 0x00, 0x85, 0xff, 0x76, 0x49, 0x81, 0xff, 0x00, 0x00, 0x04, 0x00, 0x6a, 0x00, 0x76
};

// "Encrypt"/"decrypt" a file.
void
apply_mask(char* array, size_t len)
{
  unsigned char i = 0;
  while (len--) *array++ ^= xor_mask[i++];
}

// -----------------------------------------------------------------------

void 
apply_mask(string& array, size_t start)
{
  unsigned char i = 0;
  size_t j = start;
  size_t len = array.size() - start;
  while (len--) array[j++] ^= xor_mask[i++];
}

// -----------------------------------------------------------------------

// Decompress an archived file.
void
decompress(const char* src, size_t src_len, char* dst, size_t dst_len)
{
  int bit = 1;
  const char* srcend = src + src_len;
  char* dststart = dst;
  char* dstend = dst + dst_len;
  src += 8;
  unsigned char mask = 8;
  char flag = *src++ ^ xor_mask[mask++];
  while (src < srcend && dst < dstend) {
    if (bit == 256) {
      bit = 1;
      flag = *src++ ^ xor_mask[mask++];
    }
    if (flag & bit)
      *dst++ = *src++ ^ xor_mask[mask++];
    else {
      char *repeat;
      int count = *src++ ^ xor_mask[mask++];
      count += (*src++ ^ xor_mask[mask++]) << 8;
      repeat = dst - ((count >> 4) - 1) - 1;
      count = (count & 0x0f) + 2;
      if (repeat < dststart || repeat >= dst)
        throw Error("corrupt data");
      for (int i = 0; i < count; i++)
        *dst++ = *repeat++;
    }
    bit <<= 1;
  }
}

// -----------------------------------------------------------------------

string*
compress(char* arr, size_t len)
{
  string* rv = new string;
  RealliveCompressor cmp;
  cmp.WriteData(arr, len);
  cmp.WriteDataEnd();
  cmp.Deflate();
  cmp.Flush();
  append_i32(*rv, cmp.Length());
  append_i32(*rv, len);
  rv->append(cmp.Data(), cmp.Length());
  unsigned char m = 0;
  for (unsigned int i = 0; i < rv->size(); ++i) (*rv)[i] ^= xor_mask[m++];
  return rv;
}

// -----------------------------------------------------------------------

}
}
