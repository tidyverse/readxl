#pragma once
/**
	@file
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include "util.hpp"
#include <cybozu/crypto.hpp>
#include <cybozu/stream.hpp>
#include <cybozu/string.hpp>
#include <cybozu/minixml.hpp>
#include <cybozu/endian.hpp>
#if CYBOZU_HOST == CYBOZU_HOST_INTEL
//	#define USE_CUSTOM_SHA1
#endif
#ifdef USE_CUSTOM_SHA1
#include "custom_sha1.hpp"
#endif
//#define DEBUG_CLK

#ifdef DEBUG_CLK
#define XBYAK_NO_OP_NAMES
#include <xbyak/xbyak_util.h>
#endif

namespace ms {

// [OFFCRYPTO] 2.3.4.13 encryptedVerifierHashInput step 2
static const std::string blkKey_VerifierHashInput("\xfe" "\xa7" "\xd2" "\x76" "\x3b" "\x4b" "\x9e" "\x79", 8);
// [OFFCRYPTO] 2.3.4.13 encryptedVerifierHashValue step 2
static const std::string blkKey_encryptedVerifierHashValue("\xd7" "\xaa" "\x0f" "\x6d" "\x30" "\x61" "\x34" "\x4e", 8);
// [OFFCRYPTO] 2.3.4.13 encryptedKeyValue step 2
static const std::string blkKey_encryptedKeyValue("\x14" "\x6e" "\x0b" "\xe7" "\xab" "\xac" "\xd0" "\xd6", 8);
// [OFFCRYPTO] 2.3.4.14 DataIntegrity Generation step 3
static const std::string blkKey_dataIntegrity1("\x5f" "\xb2" "\xad" "\x01" "\x0c" "\xb9" "\xe1" "\xf6", 8);
// [OFFCRYPTO] 2.3.4.14 DataIntegrity Generation step 6
static const std::string blkKey_dataIntegrity2("\xa0" "\x67" "\x7f" "\x02" "\xb2" "\x2c" "\x84" "\x33", 8);

inline void normalizeKey(std::string& key, size_t keySize)
{
	key.resize(keySize, char(0x36));
}

#ifdef DEBUG_CLK
Xbyak::util::Clock clk;
struct XXX {
	~XXX()
	{
		printf("%.1f Mclk\n", clk.getClock() / double(clk.getCount()) * 1e-6);
	}
} xxx;
#endif


inline std::string hashPassword(cybozu::crypto::Hash::Name name, const std::string& salt, const std::string& pass, int spinCount)
{
#ifdef USE_CUSTOM_SHA1
	if (name != cybozu::crypto::Hash::N_SHA1) {
		throw cybozu::Exception("hashPassword") << "not support" << cybozu::crypto::Hash::getName(name);
	}
#endif
	cybozu::crypto::Hash s(name);
	std::string h = s.digest(salt + pass);
#ifdef DEBUG_CLK
	clk.begin();
#endif
#ifdef USE_CUSTOM_SHA1
	assert(h.size() == 20);
	CustomSha1::digest(&h[0], spinCount);
#else
	for (int i = 0; i < spinCount; i++) {
		char iter[4];
		cybozu::Set32bitAsLE(iter, i);
		s.update(iter, sizeof(iter));
		s.digest(&h[0], &h[0], h.size());
	}
#endif
#ifdef DEBUG_CLK
	clk.end();
#endif
	return h;
}

#ifdef SHA1_USE_SIMD
template<int n>
inline void sha1PasswordX(std::string out[n], const std::string& salt, const std::string pass[n], int spinCount)
{
	for (int i = 0; i < n; i++) {
		out[i] = cybozu::crypto::Hash::digest(cybozu::crypto::Hash::N_SHA1, salt + pass[i]);
	}
	CustomSha1::digestX<n>(out, spinCount);
}
#endif

/*
	[MS-OFFCRYPTO] 2.3.4.10
*/
struct CipherParam {
	cybozu::crypto::Cipher::Name cipherName;
	std::string cipherNameStr;
	size_t saltSize;
	size_t blockSize;
	size_t keyBits;

	cybozu::crypto::Hash::Name hashName;
	std::string hashNameStr;
	int hashSize;
	std::string saltValue;

	CipherParam()
		: saltSize(0)
		, blockSize(0)
		, keyBits(0)
		, hashSize(0)
	{
	}
	void put() const
	{
		printf("cipherName = %s\n", cipherNameStr.c_str());
		printf("saltSize = %d\n", (int)saltSize);
		printf("blockSize = %d\n", (int)blockSize);
		printf("keyBits = %d\n", (int)keyBits);
		printf("hashName = %s\n", hashNameStr.c_str());
		printf("hashSize = %d\n", hashSize);
		printf("saltValue = "); dump(saltValue, false);
	}
	explicit CipherParam(const cybozu::minixml::Node *node)
	{
		setByXml(node);
	}
	void setByXml(const cybozu::minixml::Node *node)
	{
		saltSize = cybozu::atoi(node->attr["saltSize"]);
		blockSize = cybozu::atoi(node->attr["blockSize"]);
		keyBits = cybozu::atoi(node->attr["keyBits"]);
		hashSize = cybozu::atoi(node->attr["hashSize"]);
		saltValue = dec64(node->attr["saltValue"]);

		if (saltSize < 1 || saltSize > 65536) {
			throw cybozu::Exception("ms:CipherParam:saltSize") << saltSize;
		}
		if (blockSize < 2 || blockSize > 4096 || (blockSize & 1)) {
			throw cybozu::Exception("ms:CipherParam:blockSize") << blockSize;
		}
		const std::string& chaining = node->attr["cipherChaining"];
		cipherNameStr = node->attr["cipherAlgorithm"];

		if (cipherNameStr == "AES" && keyBits == 128 && chaining == "ChainingModeCBC") {
			cipherName = cybozu::crypto::Cipher::N_AES128_CBC;
		} else if (cipherNameStr == "AES" && keyBits == 256 && chaining == "ChainingModeCBC") {
			cipherName = cybozu::crypto::Cipher::N_AES256_CBC;
		} else {
			throw cybozu::Exception("ms:CipherParam:cipherNameStr") << cipherNameStr << keyBits << chaining;
		}
		hashNameStr = node->attr["hashAlgorithm"];

		if (hashNameStr == "SHA1" && hashSize == 20) {
			hashName = cybozu::crypto::Hash::N_SHA1;
		} else if (hashNameStr == "SHA256" && hashSize == 32) {
			hashName = cybozu::crypto::Hash::N_SHA256;
		} else if (hashNameStr == "SHA384" && hashSize == 48) {
			hashName = cybozu::crypto::Hash::N_SHA384;
		} else if (hashNameStr == "SHA512" && hashSize == 64) {
			hashName = cybozu::crypto::Hash::N_SHA512;
		} else {
			throw cybozu::Exception("ms:CipherParam:hashNameStr") << hashNameStr << hashSize;
		}
	}
	void setByName(cybozu::crypto::Cipher::Name cipherName, cybozu::crypto::Hash::Name hashName)
	{
		this->cipherName = cipherName;
		this->hashName = hashName;

		switch (cipherName) {
		case cybozu::crypto::Cipher::N_AES128_CBC:
			saltSize = 16;
			blockSize = 16;
			keyBits = 128;
			cipherNameStr = "AES";
			break;
		case cybozu::crypto::Cipher::N_AES256_CBC:
			saltSize = 16;
			blockSize = 16;
			keyBits = 256;
			cipherNameStr = "AES";
			break;
		default:
			throw cybozu::Exception("ms:CipherParam:not support cipherName") << cipherName;
		}
		if (saltSize == 0 || saltSize > 65536) throw cybozu::Exception("ms:CipherParam:setByName:bad saltSize") << saltSize;

		switch (hashName) {
		case cybozu::crypto::Hash::N_SHA1:
			hashSize = 20;
			hashNameStr = "SHA1";
			break;
		case cybozu::crypto::Hash::N_SHA256:
			hashSize = 32;
			hashNameStr = "SHA256";
			break;
		case cybozu::crypto::Hash::N_SHA384:
			hashSize = 48;
			hashNameStr = "SHA384";
			break;
		case cybozu::crypto::Hash::N_SHA512:
			hashSize = 64;
			hashNameStr = "SHA512";
			break;
		default:
			throw cybozu::Exception("ms:CipherParam:setByName:not support hash") << hashName;
		}
	}
};

} // ms

#include "standard_encryption.hpp"

namespace ms {

#ifdef __GNUC__ // defined in sys/sysmacros.h
	#undef major
	#undef minor
#endif
struct EncryptionInfo {
	int spinCount;
	uint16_t major;
	uint16_t minor;
	cybozu::MiniXml xml;
	CipherParam keyData;
	std::string encryptedHmacKey;
	std::string encryptedHmacValue;
	CipherParam encryptedKey;
	std::string encryptedVerifierHashInput;
	std::string encryptedVerifierHashValue;
	std::string encryptedKeyValue;
	// for LibreOffice
	bool isStandardEncryption;
	EncryptionHeader seHeader;
	EncryptionVerifier seVerifier;

	EncryptionInfo()
		: spinCount(0)
		, major(0)
		, minor(0)
		, isStandardEncryption(false)
	{
	}
	explicit EncryptionInfo(const std::string& data)
		: spinCount(0)
		, major(0)
		, minor(0)
		, isStandardEncryption(false)
	{
		if (data.size() < 8) {
			throw cybozu::Exception("ms:EncryptionInfo:data.size") << data.size();
		}
		const char *p = &data[0];
		major = cybozu::Get16bitAsLE(p + 0);
		minor = cybozu::Get16bitAsLE(p + 2);
		// [MS-OFFCRYPTO] 2.3.4.10
		if (major == 4 && minor == 4) {
			setAgileEncryptionInfo(data);
			return;
		}
		if ((major == 3 || major == 4) && minor == 2) {
			setStandardEncryptionInfo(data);
			isStandardEncryption = true;
			return;
		}
		throw cybozu::Exception("ms:EncryptionInfo:not support version") << major << minor;
	}
	void put() const
	{
		if (!isDebug(0)) return;
		printf("major = %d\n", major);
		printf("minor = %d\n", minor);
		printf("isStandardEncryption = %d\n", isStandardEncryption);
		if (isStandardEncryption) {
			seHeader.put();
			seVerifier.put();
		} else {
			printf("spinCount = %d\n", spinCount);
			puts("keyData");
			keyData.put();
			printf("encryptedHmacKey = "); dump(encryptedHmacKey, false);
			printf("encryptedHmacValue = "); dump(encryptedHmacValue, false);
			puts("encryptedKey");
			encryptedKey.put();
			printf("encryptedVerifierHashInput = "); dump(encryptedVerifierHashInput, false);
			printf("encryptedVerifierHashValue = "); dump(encryptedVerifierHashValue, false);
			printf("encryptedKeyValue = "); dump(encryptedKeyValue, false);
		}
	}

	void setStandardEncryptionInfo(const std::string& data)
	{
		const size_t encryptionHeaderSizePos = 8;
		size_t dataSize = data.size();
		const char *p = data.c_str();
		if (dataSize < encryptionHeaderSizePos + 4) {
			throw cybozu::Exception("ms:StandardEncryption2007Info:bad data size") << dataSize;
		}
		const uint32_t encryptionHeaderSize = cybozu::Get32bitAsLE(p + encryptionHeaderSizePos);
		if (encryptionHeaderSize > dataSize) {
			throw cybozu::Exception("ms:setStandardEncryptionInfo:bad size") << encryptionHeaderSize << dataSize;
		}
		p += encryptionHeaderSizePos + 4;
		dataSize -= encryptionHeaderSizePos + 4;
		seHeader.analyze(p, encryptionHeaderSize);
		// --- Start readxl ---
		// readxl: gate diagnostic output behind isDebug() (silent normal reads)
		// seHeader.put();
		if (isDebug()) seHeader.put();
		// --- End readxl ---

		p += encryptionHeaderSize;
		dataSize -= encryptionHeaderSize;
		// --- Start readxl ---
		// readxl: gate diagnostic output behind isDebug() (silent normal reads)
		// printf("dataSize=%u\n", (uint32_t)dataSize);
		if (isDebug()) printf("dataSize=%u\n", (uint32_t)dataSize);
		// --- End readxl ---
		seVerifier.analyze(p, dataSize);
		// --- Start readxl ---
		// readxl: gate diagnostic output behind isDebug() (silent normal reads)
		// seVerifier.put();
		if (isDebug()) seVerifier.put();
		// --- End readxl ---
	}

	void setAgileEncryptionInfo(const std::string& data)
	{
		const char *p = &data[0];
		const uint32_t reserved = cybozu::Get32bitAsLE(p + 4);
		MS_ASSERT_EQUAL(reserved, 0x40u);
		xml.parse(p + 8, p + data.size());

		// keyData
		const cybozu::minixml::Node *keyDataNode = xml.get().getFirstTagByName("keyData");
		if (keyDataNode == 0) throw cybozu::Exception("ms:EncryptionInfo:no keyData");
		keyData.setByXml(keyDataNode);
		// dataIntegrity
		const cybozu::minixml::Node *dataIntegrity = xml.get().getFirstTagByName("dataIntegrity");
		if (dataIntegrity == 0) throw cybozu::Exception("ms:EncryptionInfo:no dataIntegrity");
		encryptedHmacKey = dec64(dataIntegrity->attr["encryptedHmacKey"]);
		encryptedHmacValue = dec64(dataIntegrity->attr["encryptedHmacValue"]);

		// keyEncryptor
		const cybozu::minixml::Node *encryptedKeyNode = xml.get().getFirstTagByName("p:encryptedKey");
		if (encryptedKeyNode == 0) throw cybozu::Exception("ms:EncryptionInfo:no p:encryptedKey");
		encryptedKey.setByXml(encryptedKeyNode);
		spinCount = cybozu::atoi(encryptedKeyNode->attr["spinCount"]);
		encryptedVerifierHashInput = dec64(encryptedKeyNode->attr["encryptedVerifierHashInput"]);
		encryptedVerifierHashValue = dec64(encryptedKeyNode->attr["encryptedVerifierHashValue"]);
		encryptedKeyValue = dec64(encryptedKeyNode->attr["encryptedKeyValue"]);
	}
	std::string addHeader(const std::string& xmlStr) const
	{
		char buf[8];
		const uint16_t major = 4;
		const uint16_t minor = 4;
		const uint32_t reserved = 0x40u;
		cybozu::Set16bitAsLE(buf + 0, major);
		cybozu::Set16bitAsLE(buf + 2, minor);
		cybozu::Set32bitAsLE(buf + 4, reserved);
		return std::string(buf, sizeof(buf)) + xmlStr;
	}
	std::string toXml(bool isOffice2013 = false) const
	{
		char buf[2048];
		CYBOZU_SNPRINTF(buf, sizeof(buf),
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
			"<encryption xmlns=\"http://schemas.microsoft.com/office/2006/encryption\""
			" xmlns:p=\"http://schemas.microsoft.com/office/2006/keyEncryptor/password\""
//			" xmlns:c=\"http://schemas.microsoft.com/office/2006/keyEncryptor/certificate\""
			"%s"
			">"
			"<keyData saltSize=\"%d\" blockSize=\"%d\" keyBits=\"%d\" hashSize=\"%d\""
			" cipherAlgorithm=\"%s\" cipherChaining=\"ChainingModeCBC\" hashAlgorithm=\"%s\""
			" saltValue=\"%s\"/>"
			"<dataIntegrity encryptedHmacKey=\"%s\""
			" encryptedHmacValue=\"%s\"/><keyEncryptors>"
			"<keyEncryptor uri=\"http://schemas.microsoft.com/office/2006/keyEncryptor/password\">"
			"<p:encryptedKey spinCount=\"%d\" saltSize=\"%d\" blockSize=\"%d\" keyBits=\"%d\" hashSize=\"%d\""
			" cipherAlgorithm=\"%s\" cipherChaining=\"ChainingModeCBC\" hashAlgorithm=\"%s\""
			" saltValue=\"%s\""
			" encryptedVerifierHashInput=\"%s\""
			" encryptedVerifierHashValue=\"%s\""
			" encryptedKeyValue=\"%s\"/></keyEncryptor></keyEncryptors></encryption>",
			isOffice2013 ? " xmlns:c=\"http://schemas.microsoft.com/office/2006/keyEncryptor/certificate\"" : "",
			int(keyData.saltSize), int(keyData.blockSize), int(keyData.keyBits), int(keyData.hashSize),
			keyData.cipherNameStr.c_str(), keyData.hashNameStr.c_str(),
			enc64(keyData.saltValue).c_str(),
			enc64(encryptedHmacKey).c_str(),
			enc64(encryptedHmacValue).c_str(),
			spinCount,
			int(encryptedKey.saltSize), int(encryptedKey.blockSize), int(encryptedKey.keyBits), int(encryptedKey.hashSize),
			encryptedKey.cipherNameStr.c_str(), encryptedKey.hashNameStr.c_str(),
			enc64(encryptedKey.saltValue).c_str(),
			enc64(encryptedVerifierHashInput).c_str(),
			enc64(encryptedVerifierHashValue).c_str(),
			enc64(encryptedKeyValue).c_str()
		);
		return buf;
	}
};

inline std::string cipher(cybozu::crypto::Cipher::Name name, const char *msg, size_t msgLen, const std::string& key, const std::string& iv, cybozu::crypto::Cipher::Mode mode)
{
	cybozu::crypto::Cipher cipher(name);
	cipher.setup(mode, key, iv);

	std::string ret;
	ret.resize(msgLen + 128/* margin */);

	int roundMsgLen = msgLen & ~15;

	if (roundMsgLen > 0) {
		roundMsgLen= cipher.update(&ret[0], msg, roundMsgLen);
		if (roundMsgLen < 0) {
			throw cybozu::Exception("ms:cipher:update");
		}
	}
	const int remainSize = int(msgLen - roundMsgLen);
	if (remainSize > 0) {
		std::string remain(msg + roundMsgLen, remainSize);
		remain.resize(16);
		int writeSize = cipher.update(&ret[roundMsgLen], &remain[0], 16);
		if (writeSize < 0) {
			throw cybozu::Exception("ms:cipher:update:remain");
		}
		ret.resize(roundMsgLen + 16);
	} else {
		ret.resize(msgLen);
	}
	return ret;
}

inline std::string cipher(cybozu::crypto::Cipher::Name name, const std::string& msg, const std::string& key, const std::string& iv, cybozu::crypto::Cipher::Mode mode)
{
	return cipher(name, msg.c_str(), msg.size(), key, iv, mode);
}

inline std::string generateIv(const CipherParam& param, const std::string& blockKey, const std::string& salt)
{
	std::string ret;
	if (blockKey.empty()) {
		ret = salt;
	} else {
		ret = cybozu::crypto::Hash::digest(param.hashName, salt + blockKey);
	}
	normalizeKey(ret, param.blockSize);
	return ret;
}

/*
	[MS-OFFCRYPTO] 2.3.4.11
*/
inline std::string generateKey(const CipherParam& param, const std::string& hash, const std::string& blockKey)
{
	std::string ret = cybozu::crypto::Hash::digest(param.hashName, hash + blockKey);
	normalizeKey(ret, param.keyBits / 8);
	return ret;
}

} // ms
