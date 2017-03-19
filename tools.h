#ifndef _TOOLS_
#define _TOOLS_

class Tools
{
 public:
  static std::string b64_encode(const std::string& data);
  static std::string sha256(const std::string& data);
};

#endif // _TOOLS_H_
