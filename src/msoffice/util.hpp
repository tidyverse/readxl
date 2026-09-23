#pragma once
/**
	@file
	@brief MS Office encryption util
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include <string>
#include <cybozu/atoi.hpp>
#include <cybozu/itoa.hpp>
#include <cybozu/base64.hpp>
#include <cybozu/exception.hpp>
#include <cybozu/string.hpp>
#include <stdarg.h>

namespace ms {

enum Format {
	fZip, // not encrypted(maybe)
	fCfb, // encrypted(maybe)
	fUnknown
};

inline int& debugInstance()
{
	static int debug;
	return debug;
}

inline void setDebug(int level)
{
	debugInstance() = level;
}

inline bool& putSecretKeyInstance()
{
	static bool putSecretKey;
	return putSecretKey;
}

inline bool isDebug(int level = 1) { return debugInstance() > level; }

inline void dprintf(const char *format, ...)
{
	if (!isDebug()) return;
	va_list args;
	va_start(args, format);
#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
	vprintf(format, args);
#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
	va_end(args);
}

template<class T>
T getBlockNum(T size, T block)
{
	return (size + block - 1) / block;
}
/*
	round up size to multiple of block
*/
template<class T>
T RoundUp(T size, T block)
{
	return getBlockNum(size, block) * block;
}

inline void dump(const void *p, size_t size, bool putColon = true)
{
	const uint8_t *str = reinterpret_cast<const uint8_t*>(p);
	for (size_t i = 0; i < size; i++) {
		printf("%02X", (unsigned char)str[i]);
		if (putColon) putchar(':');
	}
	printf("\n");
}

inline void dump(const std::string& str, bool putColon = true)
{
	dump(str.c_str(), str.size(), putColon);
}

inline void dump16(const std::string& str, size_t size = size_t(-1))
{
	if (size > str.size()) size = str.size();
	for (size_t i = 0; i < size; i++) {
		printf("%02X ", (unsigned char)str[i]);
		if ((i % 16) == 15) putchar('\n');
	}
	printf("\n");
}

inline void saveFile(const std::string& file, const std::string& str)
{
	cybozu::File f(file, std::ios::out);
	f.write(str.c_str(), str.size());
}

inline std::string fromHex(const std::string& hex, bool skipColon = false)
{
	const size_t n = hex.size();
	if (!skipColon && n & 1) throw cybozu::Exception("ms:fromhex:odd length") << n;
	std::string ret;
	for (size_t i = 0; i < n; i += 2) {
		if (skipColon && hex[i] == ':') {
			i++;
			if (i == n) break;
		}
		unsigned char c = cybozu::hextoi(&hex[i], 2);
		ret += (char)c;
	}
	return ret;
}

inline std::string hex(const std::string& str)
{
	const size_t n = str.size();
	std::string ret;
	ret.reserve(n * 2);
	for (size_t i = 0; i < n; i++) {
		ret += cybozu::itohex(static_cast<uint8_t>(str[i]));
	}
	return ret;
}

inline bool dumpDiff(const std::string& x, const std::string& y)
{
	printf("dumpDiff %d %d\n", (int)x.size(), (int)y.size());
	int ccc = 0;
	for (int i = 0; i < (int)std::min(x.size(), y.size()); i++) {
		char c = x[i];
		char d = y[i];
		if (c != d) {
			printf("%8d(0x%x) %02x %02x\n", i, i, (uint8_t)c, (uint8_t)d);
			ccc++;
			if (ccc == 100) break;
		}
	}
	if (ccc == 0) printf("no diff\n");
	return ccc == 0;
}

inline std::string dec64(const std::string& str)
{
	std::string ret;
	cybozu::StringInputStream is(str);
	cybozu::StringOutputStream os(ret);
	cybozu::DecodeFromBase64(os, is);
	return ret;
}

inline std::string enc64(const std::string& str)
{
	std::string ret;
	cybozu::StringInputStream is(str);
	cybozu::StringOutputStream os(ret);
	cybozu::EncodeToBase64(os, is, 128, cybozu::base64::noEndLine);
	return ret;
}

inline void validate(bool isOK, const char *msg, const char *file, int line)
{
	if (!isOK) {
		throw cybozu::Exception("ms:cfb:validate") << msg << file << line;
	}
}

template<class L, class R>
void validateEqual(const L& a, const R& b, const char *pa, const char *pb, const char *file, int line)
{
	if (a != b) {
		throw cybozu::Exception("ms:cfb:validateEqual") << pa << a << pb << b << file << line;
	}
}

inline std::string toHex(const char *p, size_t size)
{
	std::string ret;
	for (size_t i = 0; i < size; i++) {
		cybozu::itohex(ret, static_cast<uint8_t>(p[i]), false);
	}
	return ret;
}

inline Format DetectFormat(const char *data, size_t dataSize)
{
	if (dataSize < 8) throw cybozu::Exception("ms:detectFormat:too small") << dataSize;
	if (memcmp(data, "PK\x03\x04", 4) == 0) return fZip;
	if (memcmp(data, "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1", 8) == 0) return fCfb;
	throw cybozu::Exception("ms:DetectFormat:unknown format") << hex(std::string(data, 8));
}

inline std::string Char16toChar8(const cybozu::String16& w)
{
	std::string s;
	for (size_t i = 0; i < w.size(); i++) {
		cybozu::Char16 c = w[i];
		s += char(c & 0xff);
		s += char(c >> 8);
	}
	return s;
}

#define MS_ASSERT(x) ms::validate((x), #x, __FILE__, __LINE__)
#define MS_ASSERT_EQUAL(x, y) ms::validateEqual((x), (y), #x, #y, __FILE__, __LINE__)
#define PUT(x) printf(#x "=%s(%d)\n", ms::hex(x).c_str(), (int)x.size())

} // ms
