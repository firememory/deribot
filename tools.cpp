#include <iostream>
#include <vector>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <stdexcept>

#include <boost/date_time/posix_time/posix_time.hpp>

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

std::string Tools::now_str()
{
  // Get current time from the clock, using microseconds resolution
  const boost::posix_time::ptime now =
    boost::posix_time::microsec_clock::local_time();

  // Get the time offset in current day
  const boost::posix_time::time_duration td = now.time_of_day();

  //
  // Extract hours, minutes, seconds and milliseconds.
  //
  // Since there is no direct accessor ".milliseconds()",
  // milliseconds are computed _by difference_ between total milliseconds
  // (for which there is an accessor), and the hours/minutes/seconds
  // values previously fetched.
  //
  const long hours        = td.hours();
  const long minutes      = td.minutes();
  const long seconds      = td.seconds();
  const long milliseconds = td.total_milliseconds() -
    ((hours * 3600 + minutes * 60 + seconds) * 1000);

  //
  // Format like this:
  //
  //      hh:mm:ss.SSS
  //
  // e.g. 02:15:40:321
  //
  //      ^          ^
  //      |          |
  //      123456789*12
  //      ---------10-     --> 12 chars + \0 --> 13 chars should suffice
  //
  //
  char buf[40];
  sprintf(buf, "%02ld:%02ld:%02ld.%03ld",
	  hours, minutes, seconds, milliseconds);

  return buf;
}
