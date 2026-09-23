#pragma once
/**
	@file
	@brief SHA-256, SHA-512 class
	@author MITSUNARI Shigeo(@herumi)
	@license modified new BSD license
	http://opensource.org/licenses/BSD-3-Clause
*/
/*
	use Apple CommonCrypto on macOS so that OpenSSL is not necessary
	define CYBOZU_USE_APPLE_COMMONCRYPTO=0 to disable it
*/
#include <cybozu/inttype.hpp>

#ifdef __APPLE__
	#ifndef CYBOZU_USE_APPLE_COMMONCRYPTO
		#define CYBOZU_USE_APPLE_COMMONCRYPTO 1
	#endif
#endif

/*
	use Windows CNG(bcrypt) instead of OpenSSL on Windows so that OpenSSL is not necessary
	define CYBOZU_USE_WIN_BCRYPT=0 to use OpenSSL as before
	define CYBOZU_USE_WIN_BCRYPT=1 to use bcrypt even if CYBOZU_DONT_USE_OPENSSL is defined
	@note this macro is shared with cybozu/aes.hpp
*/
#if CYBOZU_USE_APPLE_COMMONCRYPTO != 1 && defined(_WIN32)
	#if CYBOZU_USE_WIN_BCRYPT == 1
		#define CYBOZU_USE_BCRYPT_SHA
	#elif !defined(CYBOZU_USE_WIN_BCRYPT) && !defined(CYBOZU_DONT_USE_OPENSSL) && !defined(MCL_DONT_USE_OPENSSL)
		// bcrypt is used instead of OpenSSL ; CYBOZU_DONT_USE_OPENSSL selects the built-in code as before
		#define CYBOZU_USE_BCRYPT_SHA
	#endif
#endif

#if CYBOZU_USE_APPLE_COMMONCRYPTO != 1 && !defined(CYBOZU_USE_BCRYPT_SHA)
#if !defined(CYBOZU_DONT_USE_OPENSSL) && !defined(MCL_DONT_USE_OPENSSL)
	#define CYBOZU_USE_OPENSSL_SHA
#endif
#endif

#include <memory.h>

#if CYBOZU_USE_APPLE_COMMONCRYPTO == 1

#ifdef __APPLE__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <CommonCrypto/CommonDigest.h>

namespace cybozu {

class Sha256 {
	CC_SHA256_CTX ctx_;
	/*
		CC_SHA256_Update takes CC_LONG(=uint32_t) as a size,
		so a larger buffer is split.
	*/
	static const size_t maxUpdateSize_ = size_t(1) << 30;
public:
	Sha256()
	{
		clear();
	}
	void clear()
	{
		CC_SHA256_Init(&ctx_);
	}
	void update(const void *buf, size_t bufSize)
	{
		const char *p = reinterpret_cast<const char*>(buf);
		while (bufSize > maxUpdateSize_) {
			CC_SHA256_Update(&ctx_, p, CC_LONG(maxUpdateSize_));
			p += maxUpdateSize_;
			bufSize -= maxUpdateSize_;
		}
		CC_SHA256_Update(&ctx_, p, CC_LONG(bufSize));
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < CC_SHA256_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		CC_SHA256_Final(reinterpret_cast<unsigned char*>(md), &ctx_);
		return CC_SHA256_DIGEST_LENGTH;
	}
};

class Sha512 {
	CC_SHA512_CTX ctx_;
	static const size_t maxUpdateSize_ = size_t(1) << 30;
public:
	Sha512()
	{
		clear();
	}
	void clear()
	{
		CC_SHA512_Init(&ctx_);
	}
	void update(const void *buf, size_t bufSize)
	{
		const char *p = reinterpret_cast<const char*>(buf);
		while (bufSize > maxUpdateSize_) {
			CC_SHA512_Update(&ctx_, p, CC_LONG(maxUpdateSize_));
			p += maxUpdateSize_;
			bufSize -= maxUpdateSize_;
		}
		CC_SHA512_Update(&ctx_, p, CC_LONG(bufSize));
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < CC_SHA512_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		CC_SHA512_Final(reinterpret_cast<unsigned char*>(md), &ctx_);
		return CC_SHA512_DIGEST_LENGTH;
	}
};

} // cybozu

#ifdef __APPLE__
	#pragma GCC diagnostic pop
#endif

#elif defined(CYBOZU_USE_BCRYPT_SHA)

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <cybozu/exception.hpp>
#include <cybozu/inttype.hpp>
#ifdef _MSC_VER
	#pragma comment(lib, "bcrypt.lib")
#endif

namespace cybozu {

namespace sha2_local {

/*
	algorithm provider of bcrypt shared by all the instances of BCryptHash<MD_SIZE>
	because BCryptOpenAlgorithmProvider is slow (about 1500 clk) and
	an algorithm handle is thread safe.
*/
template<size_t MD_SIZE_>
struct BCryptAlg {
	PVOID volatile hAlg_; // BCRYPT_ALG_HANDLE ; PVOID for InterlockedCompareExchangePointer
	DWORD objSize_; // size of the work area of a hash object
	BCryptAlg()
		: hAlg_(0)
		, objSize_(0)
	{
	}
	~BCryptAlg()
	{
		if (hAlg_) BCryptCloseAlgorithmProvider((BCRYPT_ALG_HANDLE)hAlg_, 0);
	}
	void open(LPCWSTR algName)
	{
		if (hAlg_) return;
		BCRYPT_ALG_HANDLE hAlg = 0;
		NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, algName, NULL, 0);
		if (!BCRYPT_SUCCESS(status)) {
			throw cybozu::Exception("sha2:BCryptAlg:BCryptOpenAlgorithmProvider") << (int)status;
		}
		DWORD objSize = 0;
		DWORD writeSize = 0;
		status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(objSize), &writeSize, 0);
		if (!BCRYPT_SUCCESS(status)) {
			BCryptCloseAlgorithmProvider(hAlg, 0);
			throw cybozu::Exception("sha2:BCryptAlg:BCryptGetProperty:objectLength") << (int)status;
		}
		objSize_ = objSize;
		// another thread may open it at the same time, then close the loser
		if (InterlockedCompareExchangePointer(&hAlg_, hAlg, 0) != 0) {
			BCryptCloseAlgorithmProvider(hAlg, 0);
		}
	}
	static BCryptAlg alg_;
};

template<size_t MD_SIZE_>
BCryptAlg<MD_SIZE_> BCryptAlg<MD_SIZE_>::alg_;

/*
	SHA-256/SHA-512 by Windows CNG(bcrypt)
	the hash object is created on demand, so an instance which is not used costs nothing
*/
template<size_t MD_SIZE_>
class BCryptHash {
	static const size_t MD_SIZE = MD_SIZE_;
	/*
		BCryptHashData takes ULONG as a size, so a larger buffer is split.
	*/
	static const size_t maxUpdateSize_ = size_t(1) << 30;
	typedef BCryptAlg<MD_SIZE_> Alg;
	LPCWSTR algName_;
	BCRYPT_HASH_HANDLE hHash_;
	std::vector<uint8_t> hashObj_; // work area of hHash_ ; must be alive while hHash_ is used
	void destroyHash()
	{
		if (hHash_) {
			BCryptDestroyHash(hHash_);
			hHash_ = 0;
		}
	}
	void createHash()
	{
		if (hHash_) return;
		Alg::alg_.open(algName_);
		hashObj_.resize(Alg::alg_.objSize_);
		// CNG allocates the hash object if hashObj_ is empty
		const NTSTATUS status = BCryptCreateHash((BCRYPT_ALG_HANDLE)Alg::alg_.hAlg_, &hHash_, hashObj_.empty() ? 0 : &hashObj_[0], (ULONG)hashObj_.size(), NULL, 0, 0);
		if (!BCRYPT_SUCCESS(status)) {
			hHash_ = 0;
			throw cybozu::Exception("sha2:BCryptHash:BCryptCreateHash") << (int)status;
		}
	}
	// copy the state of rhs ; hHash_ must be zero
	void copyState(const BCryptHash& rhs)
	{
		algName_ = rhs.algName_;
		if (rhs.hHash_ == 0) return;
		hashObj_.resize(rhs.hashObj_.size());
		const NTSTATUS status = BCryptDuplicateHash(rhs.hHash_, &hHash_, hashObj_.empty() ? 0 : &hashObj_[0], (ULONG)hashObj_.size(), 0);
		if (!BCRYPT_SUCCESS(status)) {
			hHash_ = 0;
			throw cybozu::Exception("sha2:BCryptHash:BCryptDuplicateHash") << (int)status;
		}
	}
public:
	explicit BCryptHash(LPCWSTR algName)
		: algName_(algName)
		, hHash_(0)
	{
	}
	BCryptHash(const BCryptHash& rhs)
		: algName_(rhs.algName_)
		, hHash_(0)
	{
		copyState(rhs);
	}
	BCryptHash& operator=(const BCryptHash& rhs)
	{
		if (this != &rhs) {
			destroyHash();
			copyState(rhs);
		}
		return *this;
	}
	~BCryptHash()
	{
		destroyHash();
	}
	void clear()
	{
		destroyHash(); // a new hash object is created by the next update/digest
	}
	void update(const void *buf, size_t bufSize)
	{
		createHash();
		const uint8_t *p = reinterpret_cast<const uint8_t*>(buf);
		while (bufSize > 0) {
			const size_t n = bufSize > maxUpdateSize_ ? maxUpdateSize_ : bufSize;
			const NTSTATUS status = BCryptHashData(hHash_, (PUCHAR)p, (ULONG)n, 0);
			if (!BCRYPT_SUCCESS(status)) {
				throw cybozu::Exception("sha2:BCryptHash:BCryptHashData") << (int)status;
			}
			p += n;
			bufSize -= n;
		}
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < MD_SIZE) return 0;
		update(buf, bufSize);
		const NTSTATUS status = BCryptFinishHash(hHash_, (PUCHAR)md, (ULONG)MD_SIZE, 0);
		destroyHash(); // the hash object can not be reused after BCryptFinishHash
		if (!BCRYPT_SUCCESS(status)) {
			throw cybozu::Exception("sha2:BCryptHash:BCryptFinishHash") << (int)status;
		}
		return MD_SIZE;
	}
};

} // cybozu::sha2_local

class Sha256 {
	sha2_local::BCryptHash<32> ctx_;
public:
	Sha256() : ctx_(BCRYPT_SHA256_ALGORITHM) {}
	void clear() { ctx_.clear(); }
	void update(const void *buf, size_t bufSize) { ctx_.update(buf, bufSize); }
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		return ctx_.digest(md, mdSize, buf, bufSize);
	}
};

class Sha512 {
	sha2_local::BCryptHash<64> ctx_;
public:
	Sha512() : ctx_(BCRYPT_SHA512_ALGORITHM) {}
	void clear() { ctx_.clear(); }
	void update(const void *buf, size_t bufSize) { ctx_.update(buf, bufSize); }
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		return ctx_.digest(md, mdSize, buf, bufSize);
	}
};

} // cybozu

#elif defined(CYBOZU_USE_OPENSSL_SHA)

#ifndef CYBOZU_USE_OPENSSL_NEW_HASH
#ifndef _MSC_VER
#define CYBOZU_USE_OPENSSL_NEW_HASH 1
#endif
#endif

#ifdef __APPLE__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
#include <openssl/evp.h>
#include <assert.h>
#else
#include <openssl/sha.h>
#endif
#ifdef _MSC_VER
	#include <cybozu/link_libeay32.hpp>
#endif

#ifdef __APPLE__
	#pragma GCC diagnostic pop
#endif

namespace cybozu {

#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
namespace local {

template<size_t MD_SIZE_>
struct NewHash {
	static const size_t MD_SIZE = MD_SIZE_;
	EVP_MD_CTX *mdctx_;
	const EVP_MD *md_;
	explicit NewHash(const char *name)
		: mdctx_(EVP_MD_CTX_new())
		, md_(EVP_get_digestbyname(name))
	{
		if (md_ == 0) {
			fprintf(stderr, "fatal error NewHash %s\n", name);
		}
		assert(md_);
	}
	~NewHash()
	{
		EVP_MD_CTX_free(mdctx_);
	}
	void clear()
	{
		EVP_MD_CTX_reset(mdctx_);
		EVP_DigestInit_ex(mdctx_, md_, NULL);
	}
	void update(const void *buf, size_t bufSize)
	{
		EVP_DigestUpdate(mdctx_, buf, bufSize);
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < MD_SIZE) return 0;
		update(buf, bufSize);
		unsigned int len;
		EVP_DigestFinal_ex(mdctx_, (unsigned char*)md, &len);
		assert(len == MD_SIZE);
		return MD_SIZE;
	}
};

} // local
#endif

class Sha256 {
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
	local::NewHash<32> ctx_;
#else
	SHA256_CTX ctx_;
#endif
public:
	Sha256()
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		: ctx_("sha256")
#endif
	{
		clear();
	}
	void clear()
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.clear();
#else
		SHA256_Init(&ctx_);
#endif
	}
	void update(const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.update(buf, bufSize);
#else
		SHA256_Update(&ctx_, buf, bufSize);
#endif
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		return ctx_.digest(md, mdSize, buf, bufSize);
#else
		if (mdSize < SHA256_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		SHA256_Final(reinterpret_cast<uint8_t*>(md), &ctx_);
		return SHA256_DIGEST_LENGTH;
#endif
	}
};

class Sha512 {
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
	local::NewHash<64> ctx_;
#else
	SHA512_CTX ctx_;
#endif
public:
	Sha512()
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		: ctx_("sha512")
#endif
	{
		clear();
	}
	void clear()
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.clear();
#else
		SHA512_Init(&ctx_);
#endif
	}
	void update(const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		ctx_.update(buf, bufSize);
#else
		SHA512_Update(&ctx_, buf, bufSize);
#endif
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
#if CYBOZU_USE_OPENSSL_NEW_HASH == 1
		return ctx_.digest(md, mdSize, buf, bufSize);
#else
		if (mdSize < SHA512_DIGEST_LENGTH) return 0;
		update(buf, bufSize);
		SHA512_Final(reinterpret_cast<uint8_t*>(md), &ctx_);
		return SHA512_DIGEST_LENGTH;
#endif
	}
};

} // cybozu

#else

#include <cybozu/endian.hpp>
#include <memory.h>
#include <assert.h>

namespace cybozu {

namespace sha2_local {

template<class T>
T min_(T x, T y) { return x < y ? x : y;; }

inline uint32_t rot32(uint32_t x, int s)
{
#ifdef _MSC_VER
	return _rotr(x, s);
#else
	return (x >> s) | (x << (32 - s));
#endif
}

inline uint64_t rot64(uint64_t x, int s)
{
#ifdef _MSC_VER
	return _rotr64(x, s);
#else
	return (x >> s) | (x << (64 - s));
#endif
}

template<class T>
struct Common {
	void term(uint8_t *buf, size_t bufSize)
	{
		assert(bufSize < T::blockSize_);
		T& self = static_cast<T&>(*this);
		const uint64_t totalSize = self.totalSize_ + bufSize;

		buf[bufSize] = uint8_t(0x80); /* top bit = 1 */
		memset(&buf[bufSize + 1], 0, T::blockSize_ - bufSize - 1);
		if (bufSize >= T::blockSize_ - T::msgLenByte_) {
			self.round(buf);
			memset(buf, 0, T::blockSize_ - 8); // clear stack
		}
		cybozu::Set64bitAsBE(&buf[T::blockSize_ - 8], totalSize * 8);
		self.round(buf);
	}
	void inner_update(const uint8_t *buf, size_t bufSize)
	{
		T& self = static_cast<T&>(*this);
		if (bufSize == 0) return;
		if (self.roundBufSize_ > 0) {
			size_t size = sha2_local::min_(T::blockSize_ - self.roundBufSize_, bufSize);
			memcpy(self.roundBuf_ + self.roundBufSize_, buf, size);
			self.roundBufSize_ += size;
			buf += size;
			bufSize -= size;
		}
		if (self.roundBufSize_ == T::blockSize_) {
			self.round(self.roundBuf_);
			self.roundBufSize_ = 0;
		}
		while (bufSize >= T::blockSize_) {
			assert(self.roundBufSize_ == 0);
			self.round(buf);
			buf += T::blockSize_;
			bufSize -= T::blockSize_;
		}
		if (bufSize > 0) {
			assert(bufSize < T::blockSize_);
			assert(self.roundBufSize_ == 0);
			memcpy(self.roundBuf_, buf, bufSize);
			self.roundBufSize_ = bufSize;
		}
		assert(self.roundBufSize_ < T::blockSize_);
	}
};

} // cybozu::sha2_local

class Sha256 : public sha2_local::Common<Sha256> {
	friend struct sha2_local::Common<Sha256>;
private:
	static const size_t blockSize_ = 64;
	static const size_t hSize_ = 8;
	static const size_t msgLenByte_ = 8;
	uint64_t totalSize_;
	size_t roundBufSize_;
	uint8_t roundBuf_[blockSize_];
	uint32_t h_[hSize_];
	static const size_t outByteSize_ = hSize_ * sizeof(uint32_t);
	const uint32_t *k_;

	template<size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6, size_t i7>
	void round1(uint32_t *s, uint32_t *w, int i)
	{
		using namespace sha2_local;
		uint32_t e = s[i4];
		uint32_t h = s[i7];
		h += rot32(e, 6) ^ rot32(e, 11) ^ rot32(e, 25);
		uint32_t f = s[i5];
		uint32_t g = s[i6];
		h += g ^ (e & (f ^ g));
		h += k_[i];
		h += w[i];
		s[i3] += h;
		uint32_t a = s[i0];
		uint32_t b = s[i1];
		uint32_t c = s[i2];
		h += rot32(a, 2) ^ rot32(a, 13) ^ rot32(a, 22);
		h += ((a | b) & c) | (a & b);
		s[i7] = h;
	}
	/**
		@param buf [in] buffer(64byte)
	*/
	void round(const uint8_t *buf)
	{
		using namespace sha2_local;
		uint32_t w[64];
		for (int i = 0; i < 16; i++) {
			w[i] = cybozu::Get32bitAsBE(&buf[i * 4]);
		}
		for (int i = 16 ; i < 64; i++) {
			uint32_t t = w[i - 15];
			uint32_t s0 = rot32(t, 7) ^ rot32(t, 18) ^ (t >> 3);
			t = w[i - 2];
			uint32_t s1 = rot32(t, 17) ^ rot32(t, 19) ^ (t >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		uint32_t s[8];
		for (int i = 0; i < 8; i++) {
			s[i] = h_[i];
		}
		for (int i = 0; i < 64; i += 8) {
			round1<0, 1, 2, 3, 4, 5, 6, 7>(s, w, i + 0);
			round1<7, 0, 1, 2, 3, 4, 5, 6>(s, w, i + 1);
			round1<6, 7, 0, 1, 2, 3, 4, 5>(s, w, i + 2);
			round1<5, 6, 7, 0, 1, 2, 3, 4>(s, w, i + 3);
			round1<4, 5, 6, 7, 0, 1, 2, 3>(s, w, i + 4);
			round1<3, 4, 5, 6, 7, 0, 1, 2>(s, w, i + 5);
			round1<2, 3, 4, 5, 6, 7, 0, 1>(s, w, i + 6);
			round1<1, 2, 3, 4, 5, 6, 7, 0>(s, w, i + 7);
		}
		for (int i = 0; i < 8; i++) {
			h_[i] += s[i];
		}
		totalSize_ += blockSize_;
	}
public:
	Sha256()
	{
		clear();
	}
	void clear()
	{
		static const uint32_t kTbl[] = {
			0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
		};
		k_ = kTbl;
		totalSize_ = 0;
		roundBufSize_ = 0;
		h_[0] = 0x6a09e667;
		h_[1] = 0xbb67ae85;
		h_[2] = 0x3c6ef372;
		h_[3] = 0xa54ff53a;
		h_[4] = 0x510e527f;
		h_[5] = 0x9b05688c;
		h_[6] = 0x1f83d9ab;
		h_[7] = 0x5be0cd19;
	}
	void update(const void *buf, size_t bufSize)
	{
		inner_update(reinterpret_cast<const uint8_t*>(buf), bufSize);
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < outByteSize_) return 0;
		update(buf, bufSize);
		term(roundBuf_, roundBufSize_);
		char *p = reinterpret_cast<char*>(md);
		for (size_t i = 0; i < hSize_; i++) {
			cybozu::Set32bitAsBE(&p[i * sizeof(h_[0])], h_[i]);
		}
		return outByteSize_;
	}
};

class Sha512 : public sha2_local::Common<Sha512> {
	friend struct sha2_local::Common<Sha512>;
private:
	static const size_t blockSize_ = 128;
	static const size_t hSize_ = 8;
	static const size_t msgLenByte_ = 16;
	uint64_t totalSize_;
	size_t roundBufSize_;
	uint8_t roundBuf_[blockSize_];
	uint64_t h_[hSize_];
	static const size_t outByteSize_ = hSize_ * sizeof(uint64_t);
	const uint64_t *k_;

	template<size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6, size_t i7>
	void round1(uint64_t *S, const uint64_t *w, size_t i)
	{
		using namespace sha2_local;
		uint64_t& a = S[i0];
		uint64_t& b = S[i1];
		uint64_t& c = S[i2];
		uint64_t& d = S[i3];
		uint64_t& e = S[i4];
		uint64_t& f = S[i5];
		uint64_t& g = S[i6];
		uint64_t& h = S[i7];

		uint64_t s1 = rot64(e, 14) ^ rot64(e, 18) ^ rot64(e, 41);
		uint64_t ch = g ^ (e & (f ^ g));
		uint64_t t0 = h + s1 + ch + k_[i] + w[i];
		uint64_t s0 = rot64(a, 28) ^ rot64(a, 34) ^ rot64(a, 39);
		uint64_t maj = ((a | b) & c) | (a & b);
		uint64_t t1 = s0 + maj;
		d += t0;
		h = t0 + t1;
	}
	/**
		@param buf [in] buffer(64byte)
	*/
	void round(const uint8_t *buf)
	{
		using namespace sha2_local;
		uint64_t w[80];
		for (int i = 0; i < 16; i++) {
			w[i] = cybozu::Get64bitAsBE(&buf[i * 8]);
		}
		for (int i = 16 ; i < 80; i++) {
			uint64_t t = w[i - 15];
			uint64_t s0 = rot64(t, 1) ^ rot64(t, 8) ^ (t >> 7);
			t = w[i - 2];
			uint64_t s1 = rot64(t, 19) ^ rot64(t, 61) ^ (t >> 6);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		uint64_t s[8];
		for (int i = 0; i < 8; i++) {
			s[i] = h_[i];
		}
		for (int i = 0; i < 80; i += 8) {
			round1<0, 1, 2, 3, 4, 5, 6, 7>(s, w, i + 0);
			round1<7, 0, 1, 2, 3, 4, 5, 6>(s, w, i + 1);
			round1<6, 7, 0, 1, 2, 3, 4, 5>(s, w, i + 2);
			round1<5, 6, 7, 0, 1, 2, 3, 4>(s, w, i + 3);
			round1<4, 5, 6, 7, 0, 1, 2, 3>(s, w, i + 4);
			round1<3, 4, 5, 6, 7, 0, 1, 2>(s, w, i + 5);
			round1<2, 3, 4, 5, 6, 7, 0, 1>(s, w, i + 6);
			round1<1, 2, 3, 4, 5, 6, 7, 0>(s, w, i + 7);
		}
		for (int i = 0; i < 8; i++) {
			h_[i] += s[i];
		}
		totalSize_ += blockSize_;
	}
public:
	Sha512()
	{
		clear();
	}
	void clear()
	{
		static const uint64_t kTbl[] = {
		    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
		    0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
		    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
		    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
		    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
		    0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
		    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
		    0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
		    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
		    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
		    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
		    0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
		    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
		    0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
		    0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
		    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
		};
		k_ = kTbl;
		totalSize_ = 0;
		roundBufSize_ = 0;
		h_[0] = 0x6a09e667f3bcc908ull;
		h_[1] = 0xbb67ae8584caa73bull;
		h_[2] = 0x3c6ef372fe94f82bull;
		h_[3] = 0xa54ff53a5f1d36f1ull;
		h_[4] = 0x510e527fade682d1ull;
		h_[5] = 0x9b05688c2b3e6c1full;
		h_[6] = 0x1f83d9abfb41bd6bull;
		h_[7] = 0x5be0cd19137e2179ull;
	}
	void update(const void *buf, size_t bufSize)
	{
		inner_update(reinterpret_cast<const uint8_t*>(buf), bufSize);
	}
	size_t digest(void *md, size_t mdSize, const void *buf, size_t bufSize)
	{
		if (mdSize < outByteSize_) return 0;
		update(buf, bufSize);
		term(roundBuf_, roundBufSize_);
		char *p = reinterpret_cast<char*>(md);
		for (size_t i = 0; i < hSize_; i++) {
			cybozu::Set64bitAsBE(&p[i * sizeof(h_[0])], h_[i]);
		}
		return outByteSize_;
	}
};

} // cybozu

#endif

namespace cybozu {

namespace sha2_local {

/*
	HMAC (RFC 2104)
	@param out [out] must have hashSize bytes
	@param T [in] Sha256 or Sha512
*/
template<class T, size_t hashSize, size_t blockSize>
void hmac(void *out, const void *key, size_t keySize, const void *msg, size_t msgSize)
{
	const uint8_t ipad = 0x36;
	const uint8_t opad = 0x5c;
	uint8_t k[blockSize];
	T hash;
	if (keySize > blockSize) {
		hash.digest(k, hashSize, key, keySize);
		hash.clear();
		keySize = hashSize;
	} else {
		memcpy(k, key, keySize);
	}
	for (size_t i = 0; i < keySize; i++) {
		k[i] = k[i] ^ ipad;
	}
	memset(k + keySize, ipad, blockSize - keySize);
	hash.update(k, blockSize);
	hash.digest(out, hashSize, msg, msgSize);
	hash.clear();
	for (size_t i = 0; i < blockSize; i++) {
		k[i] = k[i] ^ (ipad ^ opad);
	}
	hash.update(k, blockSize);
	hash.digest(out, hashSize, out, hashSize);
}

} // cybozu::sha2_local

/*
	HMAC-SHA-256
	hmac must have 32 bytes buffer
*/
inline void hmac256(void *hmac, const void *key, size_t keySize, const void *msg, size_t msgSize)
{
	sha2_local::hmac<Sha256, 32, 64>(hmac, key, keySize, msg, msgSize);
}

/*
	HMAC-SHA-512
	hmac must have 64 bytes buffer
*/
inline void hmac512(void *hmac, const void *key, size_t keySize, const void *msg, size_t msgSize)
{
	sha2_local::hmac<Sha512, 64, 128>(hmac, key, keySize, msg, msgSize);
}

} // cybozu
