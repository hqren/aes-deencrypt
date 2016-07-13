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

#include "decrypt.h"

#include "aes.h"
#include "filters.h"
#include "modes.h"

#include "util/jni-util.h"

#define RETURN_IF_XERROR(ctx, env) \
  do { \
    jthrowable exc = (env)->ExceptionOccurred(); \
    if (exc != NULL) { \
      (env)->ExceptionClear(); \
      (env)->DeleteLocalRef(exc); \
      (ctx)->SetError("error in call to jni."); \
      return; \
    } \
  } while (false)

void DecryptPrepare(FunctionContext* ctx,
    FunctionContext::FunctionStateScope scope) {
  if (scope == FunctionContext::THREAD_LOCAL) {
    if (ctx->GetFunctionState(FunctionContext::THREAD_LOCAL) == NULL) {
      JNIEnv* env = getJNIEnv();
      jclass conf_cl = env->FindClass("org/apache/hadoop/Configuration");
      RETURN_IF_XERROR(ctx, env);
      jmethodID conf_ctor = env->GetMethodID(conf_cl, "<init>", "()V");
      jobject conf = env->NewObject(conf_cl, conf_ctor);
      RETURN_IF_XERROR(ctx, env);
      //ctx->SetFunctionState(scope, uuid_gen);
    }
  }
}

void DecryptClose(FunctionContext* ctx,
    FunctionContext::FunctionStateScope scope) {
  if (scope == FunctionContext::THREAD_LOCAL) {
    //void* uuid_gen = ctx->GetFunctionState(FunctionContext::THREAD_LOCAL);
    //DCHECK(uuid_gen != NULL);
    //delete uuid_gen;
  }
}

StringVal Decrypt(FunctionContext* context, const StringVal& arg1) {
  if (arg1.is_null) return StringVal::null();

  byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
  byte iv[CryptoPP::AES::BLOCKSIZE];
  memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
  memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

  CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);  
  CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
  std::string result_str;
  CryptoPP::StreamTransformationFilter stfDecryptor(
      cbcDecryption, new CryptoPP::StringSink(result_str));
  stfDecryptor.Put(arg1.ptr, arg1.len);
  stfDecryptor.MessageEnd();
  return StringVal::CopyFrom(context, 
      reinterpret_cast<const uint8_t*>(result_str.c_str()), result_str.size());
}

