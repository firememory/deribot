#include <iostream>
#include <vector>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <stdexcept>

#include "tools.h"

std::string Tools::b64_encode(const std::string& data)
{
  BIO* b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

  BIO* bmem = BIO_new(BIO_s_mem());
  b64 = BIO_push(b64, bmem);

  BIO_write(b64, data.c_str(), data.length());
  BIO_flush(b64);

  BUF_MEM* bptr = NULL;
  BIO_get_mem_ptr(b64, &bptr);

  std::string output(bptr->data, bptr->length);
  BIO_free_all(b64);

  return output;
}

std::string Tools::sha256(const std::string& data)
{
  unsigned char res[data.length()];

  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, data.c_str(), data.length());
  SHA256_Final(res, &ctx);

  return std::string(reinterpret_cast<const char*>(res));
}
