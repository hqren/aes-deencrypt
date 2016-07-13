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

#include "aes.h"
#include "filters.h"
#include "modes.h"

#include "util/jni-util.h"
#include "exprs/anyval-util.h"

struct AES_KEY {
  byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
  byte iv[CryptoPP::AES::BLOCKSIZE];
};

void AESPrepare(FunctionContext* context,
    FunctionContext::FunctionStateScope scope) {
  if (scope != FunctionContext::THREAD_LOCAL) return;

  AES_KEY* aes = new AES_KEY;
  memset(aes->key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
  memset(aes->iv, 0x00, CryptoPP::AES::BLOCKSIZE);
  
  StringVal* arg1 = reinterpret_cast<StringVal*>(context->GetConstantArg(1));
  if (arg1->is_null) return;
  for (int i = 0; i < arg1->len; ++i) aes->key[i % 16] ^= arg1->ptr[i];

  context->SetFunctionState(scope, aes);
}

void AESClose(FunctionContext* context,
    FunctionContext::FunctionStateScope scope) {
  if (scope != FunctionContext::THREAD_LOCAL) return;
  AES_KEY* aes = reinterpret_cast<AES_KEY*>(context->GetFunctionState(scope));
  delete aes;
}

StringVal AESDecrypt(FunctionContext* context, const StringVal& arg1,
    const StringVal& arg2) {
  // return null for empty string
  if (arg1.is_null || arg2.is_null || arg2.len > CryptoPP::AES::DEFAULT_KEYLENGTH) {
    return StringVal::null();
  }
  if (arg1.len == 0) {
    return StringVal();
  }

  AES_KEY* aes = reinterpret_cast<AES_KEY*>(context->GetFunctionState(
      FunctionContext::THREAD_LOCAL));
  std::string result_str;
  try {
    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
    d.SetKeyWithIV(aes->key, sizeof(aes->key), aes->iv);
    std::string input = impala::AnyValUtil::ToString(arg1);
    // The StreamTransformationFilter removes
    //  padding as required.
    CryptoPP::StringSource s(input, true, 
      new CryptoPP::StreamTransformationFilter(d,
        new CryptoPP::StringSink(result_str)
      ) // StreamTransformationFilter
    ); // StringSource
  } catch (const CryptoPP::Exception& e) {
    context->AddWarning(e.what());
    return StringVal::null();
  }
  return StringVal::CopyFrom(context, 
      reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());

  //CryptoPP::AES::Decryption aesDecryption(aes->key, CryptoPP::AES::DEFAULT_KEYLENGTH);  
  //CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, aes->iv);
  //std::string result_str;
  //CryptoPP::StreamTransformationFilter stfDecryptor(
  //    cbcDecryption, new CryptoPP::StringSink(result_str));
  //stfDecryptor.Put(arg1.ptr, arg1.len);
  //stfDecryptor.MessageEnd();
  //return StringVal::CopyFrom(context, 
  //    reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());
}

StringVal AESEncrypt(FunctionContext* context, const StringVal& arg1,
    const StringVal& arg2) {
  // return null for empty string
  if (arg1.is_null || arg2.is_null || arg2.len > CryptoPP::AES::DEFAULT_KEYLENGTH) {
    return StringVal::null();
  }
  if (arg1.len == 0) {
    return StringVal();
  }

  AES_KEY* aes = reinterpret_cast<AES_KEY*>(context->GetFunctionState(
      FunctionContext::THREAD_LOCAL));
  std::string result_str;
  try {
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
    e.SetKeyWithIV(aes->key, sizeof(aes->key), aes->iv);
    std::string input = impala::AnyValUtil::ToString(arg1);
    // The StreamTransformationFilter removes
    //  padding as required.
    CryptoPP::StringSource s(input, true, 
      new CryptoPP::StreamTransformationFilter(e,
        new CryptoPP::StringSink(result_str)
      ) // StreamTransformationFilter
    ); // StringSource
  } catch (const CryptoPP::Exception& e) {
    context->AddWarning(e.what());
    return StringVal::null();
  }
  return StringVal::CopyFrom(context, 
      reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());

  //CryptoPP::AES::Encryption aesEncryption(aes->key, CryptoPP::AES::DEFAULT_KEYLENGTH);  
  //CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, aes->iv);
  //std::string result_str;
  //CryptoPP::StreamTransformationFilter stfEncryptor(
  //    cbcEncryption, new CryptoPP::StringSink(result_str));
  //stfEncryptor.Put(arg1.ptr, arg1.len);
  //stfEncryptor.MessageEnd();
  //return StringVal::CopyFrom(context, 
  //    reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());
}
