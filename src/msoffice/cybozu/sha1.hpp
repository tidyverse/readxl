#pragma once
/**
	@file
	@brief SHA1 class
	@author MITSUNARI Shigeo(@herumi)
	@license modified new BSD license
	http://opensource.org/licenses/BSD-3-Clause
	@note SHA-1 is deprecated ; use SHA-256 or SHA-512 in cybozu/sha2.hpp
*/
/*
	the backend (Apple CommonCrypto / Windows CNG(bcrypt) / OpenSSL / built-in)
	is selected by the same macros as cybozu/sha2.hpp
	(CYBOZU_USE_APPLE_COMMONCRYPTO, CYBOZU_USE_WIN_BCRYPT, CYBOZU_DONT_USE_OPENSSL)
*/
#include <cybozu/sha2.hpp>

#if CYBOZU_USE_APPLE_COMMONCRYPTO == 1

#ifdef __APPLE__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace cybozu {

class Sha1 {
	CC_SHA1_CTX ctx_;
	/*
		CC_SHA1_Update takes CC_LONG(=uint32_t) as a size,
		so a larger buffer is split.
	*/
	static const size_t maxUpdateSize_ = size_t(1) << 30;
public:
	Sha1()
	{
		clear();
	}
	void clear()
	{
		CC_SHA1_Init(&ctx_);
	}
	void update(const void *buf, size_t bufSize)
	{
		const char *p = reinterpret_cast<const char*>(buf);
		while (bufSize > maxUpdateSize_) {
			CC_SHA1_Update(&ctx_, p, CC_LONG(maxUpdateSize_));
			p += maxUpdateSize_;
			bufSize -= maxUpdateSize_;
		}
		CC_SHA1_Update(&ctx_, p, CC_LONG(bufSize));
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < CC_SHA1_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		CC_SHA1_Final(reinterpret_cast<unsigned char*>(md), &ctx_);
		return CC_SHA1_DIGEST_LENGTH;
	}
};

} // cybozu

#ifdef __APPLE__
	#pragma GCC diagnostic pop
#endif

#elif defined(CYBOZU_USE_BCRYPT_SHA)

namespace cybozu {

class Sha1 {
	sha2_local::BCryptHash<20> ctx_;
public:
	Sha1() : ctx_(BCRYPT_SHA1_ALGORITHM) {}
	void clear() { ctx_.clear(); }
	void update(const void *buf, size_t bufSize) { ctx_.update(buf, bufSize); }
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		return ctx_.digest(md, mdSize, buf, bufSize);
	}
};

} // cybozu

#elif defined(CYBOZU_USE_OPENSSL_SHA)

namespace cybozu {

class Sha1 {
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
	local::NewHash<20> ctx_;
#else
	SHA_CTX ctx_;
#endif
public:
	Sha1()
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		: ctx_("sha1")
#endif
	{
		clear();
	}
	void clear()
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.clear();
#else
		SHA1_Init(&ctx_);
#endif
	}
	void update(const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.update(buf, bufSize);
#else
		SHA1_Update(&ctx_, buf, bufSize);
#endif
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		return ctx_.digest(md, mdSize, buf, bufSize);
#else
		if (mdSize < SHA_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		SHA1_Final(reinterpret_cast<uint8_t*>(md), &ctx_);
		return SHA_DIGEST_LENGTH;
#endif
	}
};

} // cybozu

#else

#include <cybozu/endian.hpp>
#include <algorithm>
#include <memory.h>
#include <assert.h>

namespace cybozu {

class Sha1 {
private:
	uint64_t totalSize_;
	size_t roundBufSize_;
	char roundBuf_[64];
	uint32_t H_[5];
	uint32_t K_[80];
	uint32_t digest_[5];
	bool done_;

	uint32_t S(uint32_t x, int s) const
	{
#ifdef _MSC_VER
		return _rotl(x, s);
#else
		return (x << s) | (x >> (32 - s));
#endif
	}

	uint32_t f0(uint32_t b, uint32_t c, uint32_t d) const { return (b & c) | (~b & d); }
	uint32_t f1(uint32_t b, uint32_t c, uint32_t d) const { return b ^ c ^ d; }
	uint32_t f2(uint32_t b, uint32_t c, uint32_t d) const { return (b & c) | (b & d) | (c & d); }
	uint32_t f(int t, uint32_t b, uint32_t c, uint32_t d) const
	{
		if (t < 20) {
			return f0(b, c, d);
		} else
		if (t < 40) {
			return f1(b, c, d);
		} else
		if (t < 60) {
			return f2(b, c, d);
		} else {
			return f1(b, c, d);
		}
	}

	/**
		@param buf [in] buffer(64byte)
	*/
	void round(const char *buf)
	{
		uint32_t W[80];
		for (int i = 0; i < 16; i++) {
			W[i] = cybozu::Get32bitAsBE(&buf[i * 4]);
		}
		for (int i = 16 ; i < 80; i++) {
			W[i] = S(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
		}
		uint32_t a = H_[0];
		uint32_t b = H_[1];
		uint32_t c = H_[2];
		uint32_t d = H_[3];
		uint32_t e = H_[4];
		for (int i = 0; i < 80; i++) {
			uint32_t tmp = S(a, 5) + f(i, b, c, d) + e + W[i] + K_[i];
			e = d;
			d = c;
			c = S(b, 30);
			b = a;
			a = tmp;
		}
		H_[0] += a;
		H_[1] += b;
		H_[2] += c;
		H_[3] += d;
		H_[4] += e;
		totalSize_ += 64;
	}
	/*
		final phase
		@note bufSize < 64
	*/
	void term(const char *buf, size_t bufSize)
	{
		assert(bufSize < 64);
		const uint64_t totalSize = totalSize_ + bufSize;

		uint8_t last[64];
		memcpy(last, buf, bufSize);
		memset(&last[bufSize], 0, 64 - bufSize);
		last[bufSize] = uint8_t(0x80); /* top bit = 1 */
		if (bufSize >= 56) {
			round(cybozu::cast<const char*>(last));
			memset(last, 0, sizeof(last)); // clear stack
		}
		cybozu::Set32bitAsBE(&last[56], uint32_t(totalSize >> 29));
		cybozu::Set32bitAsBE(&last[60], uint32_t(totalSize * 8));
		round(cybozu::cast<const char*>(last));

		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(H_); i++) {
			cybozu::Set32bitAsBE(&digest_[i], H_[i]);
		}
		done_ = true;
	}
public:
	Sha1()
	{
		clear();
	}
	void clear()
	{
		static const uint32_t tbl[] = {
			0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6
		};
		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
			for (int j = 0; j < 20; j++) {
				K_[i * 20 + j] = tbl[i];
			}
		}
		totalSize_ = 0;
		roundBufSize_ = 0;
		H_[0] = 0x67452301;
		H_[1] = 0xefcdab89;
		H_[2] = 0x98badcfe;
		H_[3] = 0x10325476;
		H_[4] = 0xc3d2e1f0;
		done_ = false;
	}
	void update(const void *_buf, size_t bufSize)
	{
		const char *buf = (const char *)_buf;
		if (bufSize == 0) return;
		assert(!done_);
		if (roundBufSize_ > 0) {
			size_t size = std::min(64 - roundBufSize_, bufSize);
			memcpy(roundBuf_ + roundBufSize_, buf, size);
			roundBufSize_ += size;
			buf += size;
			bufSize -= size;
		}
		if (roundBufSize_ == 64) {
			round(roundBuf_);
			roundBufSize_ = 0;
		}
		while (bufSize >= 64) {
			assert(roundBufSize_ == 0);
			round(buf);
			buf += 64;
			bufSize -= 64;
		}
		if (bufSize > 0) {
			assert(bufSize < 64);
			assert(roundBufSize_ == 0);
			memcpy(roundBuf_, buf, bufSize);
			roundBufSize_ = bufSize;
		}
		assert(roundBufSize_ < 64);
	}
	size_t digest(void *md, size_t mdSize, const void *_buf, size_t bufSize)
	{
		const char *buf = (const char *)_buf;
		assert(!done_);
		update(buf, bufSize);
		term(roundBuf_, roundBufSize_);
		const size_t outByteSize = sizeof(digest_);
		if (mdSize < outByteSize) return 0;
		memcpy(md, digest_, outByteSize);
		clear();
		return outByteSize;
	}
};

} // cybozu

#endif

namespace cybozu {

/*
	HMAC-SHA-1 (deprecated)
	hmac must have 20 bytes buffer
*/
inline void hmac1(void *hmac, const void *key, size_t keySize, const void *msg, size_t msgSize)
{
	sha2_local::hmac<Sha1, 20, 64>(hmac, key, keySize, msg, msgSize);
}

} // cybozu
