/*
 * hmac_md5.h
 *
 */

#ifndef BASE_HMAC_MD5_H_
#define BASE_HMAC_MD5_H_

#include "md5.h"

// HMAC_MD5Calc (MD5-based Message Authentication Code)

class HMAC_MD5Calc {
public:
	HMAC_MD5Calc(const void* key, size_t key_len);

	void update(const void* inbuf, size_t len);

	void final(MD5Digest* digest);
	string final();

private:
	char hmac_ctx_[64];
	MD5Calc md5_;
};

void HMAC_MD5Sum(const void* data, size_t length, const void* key,
		size_t key_len, MD5Digest* digest);

string HMAC_MD5Sum(const void* data, size_t length, const void* key,
		size_t key_len);

string HMAC_MD5String(boost::string_ref str, boost::string_ref key);


#endif /* BASE_HMAC_MD5_H_ */
