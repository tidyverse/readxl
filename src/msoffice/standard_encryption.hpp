#pragma once
/**
	@file
	@brief MS Office 2007 Standard Encryption decoder
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include "util.hpp"
#include <cybozu/crypto.hpp>
#include <cybozu/string.hpp>
#include <cybozu/endian.hpp>

namespace ms {

/*
	Office 2007
	[MS-OFFCRYPTO] 2.3.2
*/
struct EncryptionHeader {
	uint32_t flags;
	uint32_t sizeExtra;
	uint32_t algId;
	uint32_t algIdHash;
	uint32_t keySize;
	uint32_t providerType;
	uint32_t reserved1;
	uint32_t reserved2;
	cybozu::String16 cspName;
	cybozu::crypto::Cipher::Name cipherName;
	enum AlgoMode {
		AlgoAny,
		AlgoRC4,
		Algo128AES,
		Algo192AES,
		Algo256AES
	};
	void analyze(const char *data, size_t dataSize)
	{
		if (dataSize < 32) {
			throw cybozu::Exception("ms:EncryptionHeader:shot dataSize") << dataSize;
		}
		uint32_t *tbl[] = {
			&flags, &sizeExtra, &algId, &algIdHash, &keySize, &providerType,
			&reserved1, &reserved2
		};
		const size_t paraNum = 8;
		assert(CYBOZU_NUM_OF_ARRAY(tbl) == paraNum);
		for (size_t i = 0; i < paraNum; i++) {
			*(tbl[i]) = cybozu::Get32bitAsLE(data + i * 4);
		}
		cybozu::Char16 c = cybozu::Get16bitAsLE(data + dataSize - 2);
		if (c != 0) {
			throw cybozu::Exception("ms:EncryptionHeader:cspName is not NULL terminated") << c;
		}
		cspName.assign(reinterpret_cast<const cybozu::Char16*>(data + paraNum * 4));
		MS_ASSERT_EQUAL(sizeExtra, 0u);
		switch (algId) {
//		case 0x0000: cipherName = AlgoAny; break;
//		case 0x6801: cipherName = AlgoRC4; break;
		case 0x660e: cipherName =  cybozu::crypto::Cipher::N_AES128_ECB; break;
		case 0x660f: cipherName =  cybozu::crypto::Cipher::N_AES192_ECB; break;
		case 0x6610: cipherName =  cybozu::crypto::Cipher::N_AES256_ECB; break;
		default:
			throw cybozu::Exception("ms:EncryptionHeader:bad algId") << algId;
		}
		// verify keySize
		MS_ASSERT_EQUAL(keySize, cybozu::crypto::Cipher::getSize(cipherName));
		MS_ASSERT(algIdHash == 0u || algIdHash == 0x8004);
		// verify providerType
		MS_ASSERT_EQUAL(providerType, 0x18u);
		MS_ASSERT_EQUAL(reserved2, 0u);
	}
	void put() const
	{
		printf("flags = %08x\n", flags);
		printf("sizeExtra = %u\n", sizeExtra);
		printf("algId = %08x\n", algId);
		printf("algIdHash = %08x\n", algIdHash);
		printf("keySize = %u\n", keySize);
		printf("providerType = %08x\n", providerType);
		printf("cspName = %s\n", cybozu::ToUtf8(cspName).c_str());
	}
};

/*
	[MS-OFFCRYPTO] 2.3.3
*/
struct EncryptionVerifier {
	static const size_t bufSize = 16;
	uint32_t saltSize;
	std::string salt; // bufSize
	std::string encryptedVerifier; // bufSize
	uint32_t verifierHashSize;
	std::string encryptedVerifierHash;


	void analyze(const char *data, size_t dataSize)
	{
		const size_t saltSizePos = 0;
		const size_t saltPos = saltSizePos + sizeof(saltSize);
		const size_t encryptedVerifierPos = saltPos + bufSize;
		const size_t verifierHashSizePos = encryptedVerifierPos + bufSize;
		const size_t encryptedVerifierHashPos = verifierHashSizePos + sizeof(verifierHashSize);

		if (dataSize < encryptedVerifierHashPos) {
			throw cybozu::Exception("ms:EncryptionVerifier:dataSize is too small") << dataSize << encryptedVerifierHashPos;
		}
		saltSize = cybozu::Get32bitAsLE(data + saltSizePos);
		MS_ASSERT_EQUAL(saltSize, 0x10u);
		salt.assign(data + saltPos, bufSize);
		encryptedVerifier.assign(data + encryptedVerifierPos, bufSize);
		verifierHashSize = cybozu::Get32bitAsLE(data + verifierHashSizePos);
		if (verifierHashSize + sizeof(verifierHashSize) > dataSize) {
			throw cybozu::Exception("ms:EncryptionVerifier:bad verifierHashSize") << verifierHashSize << dataSize;
		}
		encryptedVerifierHash.assign(data + encryptedVerifierHashPos, data + dataSize);
	}
	/*
		2.3.4.7
	*/
	std::string getEncryptionKey(const std::string& pass) const
	{
		cybozu::crypto::Hash::Name hashName = cybozu::crypto::Hash::N_SHA1;
		const size_t hashSize = cybozu::crypto::Hash::getSize(hashName);
		std::string h = hashPassword(hashName, salt, pass, 50000);
		const std::string zero4byte(4, 0); // 0x00000000;
		h = cybozu::crypto::Hash::digest(hashName, h + zero4byte);

		std::string t(64, 0x36); // '\x36...\x36'(64 times)
		for (size_t i = 0; i < hashSize; i++) {
			t[i] ^= h[i];
		}
		h = cybozu::crypto::Hash::digest(hashName, t);
		return h;
	}
	void put() const
	{
		printf("saltSize = %d\n", saltSize);
		printf("salt = "); ms::dump(salt);
		printf("encryptedVerifier = "); ms::dump(encryptedVerifier);
		printf("verifierHashSize = %d\n", verifierHashSize);
		printf("encryptedVerifierHash = "); ms::dump(encryptedVerifierHash);
	}
};

} // ms
