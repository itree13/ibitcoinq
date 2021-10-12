/*
 * md5.h
 *
 */

#ifndef BASE_MD5_H_
#define BASE_MD5_H_

#include "../../type.h"
#include <boost/utility/string_ref.hpp>


// The output of an MD5 operation.
struct MD5Digest {
	unsigned char d[16];
};

class MD5Calc {
public:
	MD5Calc();

	void update(const void* inbuf, size_t len);

	void final(MD5Digest* digest);
	string final();

private:
	char ctx_[88];
};

// Computes the MD5 sum of the given data buffer with the given length.
void MD5Sum(const void* data, size_t length, MD5Digest* digest);

// Computes the MD5 and return human-readable hexadecimal.
string MD5Sum(const void* data, size_t length);
string MD5String(boost::string_ref str);

// Converts a digest into human-readable hexadecimal.
string MD5DigestToBase16(const MD5Digest& digest);

#endif /* BASE_MD5_H_ */
