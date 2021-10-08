#ifndef _BASE_BASE64_H_
#define _BASE_BASE64_H_
  
#include "../type.h"
#include <boost/utility/string_ref.hpp>


string BASE64Encode(boost::string_ref str);
string BASE64Decode(boost::string_ref str);


#endif
