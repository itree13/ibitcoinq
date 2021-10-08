/*
 * hmac_md5.cpp
 *
 */

#include "hmac_md5.h"


HMAC_MD5Calc::HMAC_MD5Calc(const void* key, size_t key_len) {
	MD5Digest md5dig;
	if (key_len > 64) {
		MD5Sum(key, key_len, &md5dig);

		key = md5dig.d;
		key_len = 16;
	}

	unsigned char inner_padding[64];
	memset(inner_padding, 0, 64);
	memset(hmac_ctx_, 0, 64);
	memcpy(inner_padding, key, key_len);
	memcpy(hmac_ctx_, key, key_len);

	for (int i = 0; i < 64; ++i) {
		inner_padding[i] ^= 0x36;
		hmac_ctx_[i] ^= 0x5c;
	}

	md5_.update(inner_padding, 64);
}

void HMAC_MD5Calc::update(const void* inbuf, size_t len) {
	md5_.update(inbuf, len);
}

void HMAC_MD5Calc::final(MD5Digest* digest) {
	md5_.final(digest);

	MD5Calc md5;
	md5.update(hmac_ctx_, 64);
	md5.update(digest->d, 16);
	md5.final(digest);
}

string HMAC_MD5Calc::final() {
	MD5Digest dig;
	final(&dig);
	return MD5DigestToBase16(dig);
}

void HMAC_MD5Sum(const void* data, size_t length, const void* key,
		size_t key_len, MD5Digest* digest) {
	HMAC_MD5Calc calc(key, key_len);
	calc.update(data, length);
	calc.final(digest);
}

string HMAC_MD5Sum(const void* data, size_t length, const void* key,
		size_t key_len) {
	MD5Digest digest;
	HMAC_MD5Sum(data, length, key, key_len, &digest);
	return MD5DigestToBase16(digest);
}

string HMAC_MD5String(boost::string_ref str, boost::string_ref key) {
	return HMAC_MD5Sum(str.data(), str.size(), key.data(), key.size());
}
