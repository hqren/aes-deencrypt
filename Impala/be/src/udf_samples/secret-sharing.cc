// Copyright 2012 Cloudera Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "aes-deencrypt.h"

extern "C" {

#include "libgfshare.h"

}

#include "exprs/math-functions.h"

#include <string>

#define BUFFER_SIZE 4096

StringVal SS_Split(FunctionContext* context, const IntVal& arg1, const IntVal& arg2,
    const StringVal& arg3) {
  gfshare_ctx *G;
  int sharecount = arg1.val;
  int threshold = arg2.val;
  int keylen = arg3.len;
  unsigned char* sharenrs = reinterpret_cast<unsigned char*>(malloc(sharecount));
  for(int i = 0; i < sharecount; ++i) {
    unsigned char proposed = (random() & 0xff00) >> 8;
    if( proposed == 0 ) {
      proposed = 1;
    }
    SHARENR_TRY_AGAIN:
    for(int j = 0; j < i; ++j ) {
      if( sharenrs[j] == proposed ) {
        proposed++;
        if( proposed == 0 ) proposed = 1;
        goto SHARENR_TRY_AGAIN;
      }
    }
    sharenrs[i] = proposed;
  }
  G = gfshare_ctx_init_enc(sharenrs, sharecount, threshold, std::min(BUFFER_SIZE, keylen));
  gfshare_ctx_enc_setsecret(G, arg3.ptr);
  StringVal buffer(context, BUFFER_SIZE);
  std::string result_str;
  for(int i = 0; i < sharecount; ++i) {
    gfshare_ctx_enc_getshare(G, i, buffer.ptr);
    result_str += impala::MathFunction::HexString(context, buffer);
    result_str += "\n";
  }
  return StringVal::CopyFrom(context, 
      reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());
}

StringVal SS_Combine(FunctionContext* context, const StringVal& arg1,
    int num_var_args, const StringVal* args) {
  int sharecount = num_var_args;
  unsigned char* sharenrs = reinterpret_cast<unsigned char*>(malloc(sharecount));
  for(int i = 0; i < sharecount; ++i) {
    unsigned char proposed = (random() & 0xff00) >> 8;
    if( proposed == 0 ) {
      proposed = 1;
    }
    SHARENR_TRY_AGAIN:
    for(int j = 0; j < i; ++j ) {
      if( sharenrs[j] == proposed ) {
        proposed++;
        if( proposed == 0 ) proposed = 1;
        goto SHARENR_TRY_AGAIN;
      }
    }
    sharenrs[i] = proposed;
  }
  gfshare_ctx *G;
  G = gfshare_ctx_init_dec(sharenrs, sharecount, BUFFER_SIZE);
  for (int i = 0; i < sharecount; ++i) {
    gfshare_ctx_dec_giveshare(G, i, args[i].ptr);
  }
  gfshare_ctx_dec_extract(G, args[0].ptr);
  gfshare_ctx_free(G);

  return StringVal::CopyFrom(context, args[0].ptr, args[0].len);
}
