#pragma once
/**
	@file
	@brief wrapper of cybozu/sha1.hpp, cybozu/sha2.hpp and cybozu/aes.hpp
	@author MITSUNARI Shigeo(@herumi)
*/

#include <cybozu/exception.hpp>
#include <cybozu/inttype.hpp>
#include <cybozu/aes.hpp>
#include <cybozu/sha1.hpp>
#include <cybozu/sha2.hpp>

namespace cybozu {

namespace crypto {

/*
	SHA-1, SHA-256 and SHA-512 are supported (SHA-1 is deprecated).
	SHA-224 and SHA-384 are not supported ; getSize() and getName() still
	accept them for compatibility, but constructing Hash/Hmac with them throws.
*/
class Hash {
public:
	enum Name {
		N_SHA1, // deprecated
		N_SHA224, // deprecated
		N_SHA256,
		N_SHA384, // deprecated
		N_SHA512
	};
private:
	Name name_;
	size_t hashSize_;
	Sha1 sha1_;
	Sha256 sha256_;
	Sha512 sha512_;
public:
	static inline size_t getSize(Name name)
	{
		switch (name) {
		case N_SHA1:   return 160 / 8;
		case N_SHA224: return 224 / 8;
		case N_SHA256: return 256 / 8;
		case N_SHA384: return 384 / 8;
		case N_SHA512: return 512 / 8;
		default:
			throw cybozu::Exception("crypto:Hash:getSize") << name;
		}
	}
	static inline const char *getName(Name name)
	{
		switch (name) {
		case N_SHA1:   return "sha1";
		case N_SHA224: return "sha224";
		case N_SHA256: return "sha256";
		case N_SHA384: return "sha384";
		case N_SHA512: return "sha512";
		default:
			throw cybozu::Exception("crypto:Hash:getName") << name;
		}
	}
	static inline Name getName(const std::string& nameStr)
	{
		static const struct {
			const char *nameStr;
			Name name;
		} tbl[] = {
			{ "sha1", N_SHA1 },
			{ "sha224", N_SHA224 },
			{ "sha256", N_SHA256 },
			{ "sha384", N_SHA384 },
			{ "sha512", N_SHA512 },
		};
		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
			if (nameStr == tbl[i].nameStr) return tbl[i].name;
		}
		throw cybozu::Exception("crypto:Hash:getName") << nameStr;
	}
	/*
		throw if name is not N_SHA1, N_SHA256 nor N_SHA512
	*/
	static inline void verifyName(Name name)
	{
		if (name != N_SHA1 && name != N_SHA256 && name != N_SHA512) {
			throw cybozu::Exception("crypto:Hash:not supported") << name;
		}
	}
	explicit Hash(Name name)
		: name_(name)
		, hashSize_(getSize(name))
	{
		verifyName(name_);
		reset();
	}
	void update(const void *buf, size_t bufSize)
	{
		switch (name_) {
		case N_SHA1:   sha1_.update(buf, bufSize); break;
		case N_SHA256: sha256_.update(buf, bufSize); break;
		case N_SHA512: sha512_.update(buf, bufSize); break;
		default:
			throw cybozu::Exception("crypto:Hash:update") << name_;
		}
	}
	void update(const std::string& buf)
	{
		update(buf.c_str(), buf.size());
	}
	void reset()
	{
		switch (name_) {
		case N_SHA1:   sha1_.clear(); break;
		case N_SHA256: sha256_.clear(); break;
		case N_SHA512: sha512_.clear(); break;
		default:
			throw cybozu::Exception("crypto:Hash:reset") << name_;
		}
	}
	/*
		md must have hashSize byte
		@note clear inner buffer after calling digest
	*/
	void digest(void *out, const void *buf, size_t bufSize)
	{
		switch (name_) {
		case N_SHA1:   sha1_.digest(out, hashSize_, buf, bufSize); break;
		case N_SHA256: sha256_.digest(out, hashSize_, buf, bufSize); break;
		case N_SHA512: sha512_.digest(out, hashSize_, buf, bufSize); break;
		default:
			throw cybozu::Exception("crypto:Hash:digest") << name_;
		}
		reset();
	}
	std::string digest(const void *buf, size_t bufSize)
	{
		std::string ret;
		ret.resize(hashSize_);
		digest(&ret[0], buf, bufSize);
		return ret;
	}
	std::string digest(const std::string& buf = "")
	{
		return digest(buf.c_str(), buf.size());
	}
	/*
		out must have necessary size
		@note return written size
	*/
	static inline size_t digest(void *out, Name name, const void *buf, size_t bufSize)
	{
		Hash h(name);
		h.digest(out, buf, bufSize);
		return getSize(name);
	}
	static inline std::string digest(Name name, const void *buf, size_t bufSize)
	{
		char md[128];
		size_t size = digest(md, name, buf, bufSize);
		if (size == 0) throw cybozu::Exception("crypt:Hash:digest") << name;
		return std::string(md, size);
	}
	static inline std::string digest(Name name, const std::string& buf)
	{
		return digest(name, buf.c_str(), buf.size());
	}
};

class Hmac {
	Hash::Name name_;
	size_t hashSize_;
public:
	explicit Hmac(Hash::Name name)
		: name_(name)
		, hashSize_(Hash::getSize(name))
	{
		Hash::verifyName(name_);
	}
	size_t getSize() const { return hashSize_; }
	/*
		out must have getSize() byte
	*/
	void eval(void *out, const void *key, size_t keySize, const void *msg, size_t msgSize) const
	{
		switch (name_) {
		case Hash::N_SHA1:   cybozu::hmac1(out, key, keySize, msg, msgSize); break;
		case Hash::N_SHA256: cybozu::hmac256(out, key, keySize, msg, msgSize); break;
		case Hash::N_SHA512: cybozu::hmac512(out, key, keySize, msg, msgSize); break;
		default:
			throw cybozu::Exception("crypto:Hmac:eval") << name_;
		}
	}
	std::string eval(const std::string& key, const std::string& data) const
	{
		std::string out(hashSize_, 0);
		eval(&out[0], key.c_str(), key.size(), data.c_str(), data.size());
		return out;
	}
};

} }	// cybozu::crypto
