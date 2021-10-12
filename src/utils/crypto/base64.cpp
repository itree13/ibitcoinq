#include "base64.h"


static const char *g_pCodes =  
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";  
  
static const unsigned char g_pMap[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
     52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
    255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
      7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
     19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
     37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
     49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255  
};  

string BASE64Encode(const void* data, size_t size) {
	string out;

	const unsigned char* pIn = (const unsigned char*)data;
    
	size_t leven = 3 * (size / 3);

	size_t i = 0;
    for(; i < leven; i += 3) {
		out += g_pCodes[pIn[0] >> 2];
		out += g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		out += g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		out += g_pCodes[pIn[2] & 0x3f];
        pIn += 3;  
    }  
  
	if (i < size) {
        unsigned char a = pIn[0];  
		unsigned char b = ((i + 1) < size) ? pIn[1] : 0;
        unsigned char c = 0;  
  
		out += g_pCodes[a >> 2];
		out += g_pCodes[((a & 3) << 4) + (b >> 4)];
		out += ((i + 1) < size) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		out += '=';
    }
	return out;
}

string BASE64Encode(boost::string_ref str) {
	return BASE64Encode(str.data(), str.size());
}

string BASE64Decode(boost::string_ref str) {
	string out;

    unsigned long t, x, y, z;  
    unsigned char c;  
    unsigned long g = 3;  
   
    for(x = y = z = t = 0; x < str.length(); ++x)  {
        c = g_pMap[(unsigned)str[x]];
        if(c == 255) continue;  
        if(c == 254) { c = 0; g--; }  
  
        t = (t << 6) | c;  
  
        if(++y == 4)  
        {
			out += (char)((t >> 16) & 255);
			if (g > 1) out += (char)((t >> 8) & 255);
			if (g > 2) out += (char)(t & 255);
            y = t = 0;  
        }  
	}
	return out;
}
