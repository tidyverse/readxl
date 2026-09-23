#pragma once
/**
	@file
	@brief compound file binary format
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include <assert.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cybozu/endian.hpp>
#include <cybozu/time.hpp>
#include <cybozu/string.hpp>
#include "util.hpp"

namespace ms { namespace cfb {

typedef std::vector<uint32_t> UintVec;
typedef std::map<uint32_t, UintVec> ChainMap;

const uint32_t MAXREGSECT = 0xfffffffa;
const uint32_t DIFSECT = 0xfffffffc;
const uint32_t FATSECT = 0xfffffffd;
const uint32_t ENDOFCHAIN = 0xfffffffe;
const uint32_t FREESECT = 0xffffffff;
const uint32_t NOSTREAM = 0xffffffff;

enum ObjectType {
	Unallocated = 0,
	StorageObject = 1,
	StreamObject = 2,
	RootStorageObject = 5
};
enum ColorFlag {
	Red = 0,
	Black = 1
};

inline std::string toStr(uint32_t v)
{
	switch (v) {
	case DIFSECT:
		return "DIF";
	case FATSECT:
		return "FAT";
	case ENDOFCHAIN:
		return "END";
	case FREESECT:
		return "FREE";
	default:
		return cybozu::itoa(v);
	}
}

inline std::string toStr(ObjectType objectType)
{
	switch (objectType) {
	case Unallocated:
		return "Unallocated";
	case StorageObject:
		return "StorageObject";
	case StreamObject:
		return "StreamObject";
	case RootStorageObject:
		return "RootStorageObject";
	default:
		throw cybozu::Exception("invalid ObjectType");
	}
}

inline std::string toStr(ColorFlag colorFlag)
{
	switch (colorFlag) {
	case Red:
		return "Red";
	case Black:
		return "Black";
	default:
		throw cybozu::Exception("invalid ColorFlag");
	}
}

template<class T>
inline void putCompact(const T& vec)
{
	uint32_t prev = 0xfffffff0;
	bool isFirst = true;
	for (size_t j = 0, n = vec.size(); j < n; j++) {
		uint32_t v = vec[j];
		if (v == prev + 1 && j != n - 1) {
			if (isFirst) {
				printf(" ..");
				isFirst = false;
			}
		} else {
			if (!isFirst && j > 1) {
				printf(" %s", toStr(vec[j - 1]).c_str());
			}
			printf(" %s", toStr(v).c_str());
			isFirst = true;
		}
		prev = v;
	}
	putchar('\n');
}

struct Header {
	enum { firstNumDIFAT = 109 };
	uint32_t minorVersion;
	uint32_t majorVersion;
	uint32_t sectorShift;
	uint32_t numDirectorySectors;
	uint32_t numFatSectors;
	uint32_t firstDirectorySectorLocation;
	uint32_t transactionSignatureNumber;
	uint32_t firstMiniFatSectorLocation;
	uint32_t numMiniFatSectors;
	uint32_t firstDifatSectorLocation;
	uint32_t numDifatSectors;
	UintVec difat;
	uint32_t sectorSize;
	explicit Header(const char *data = 0, uint32_t dataSize = 0)
		: minorVersion(0x003e)
		, majorVersion(3)
		, sectorShift(9)
		, numDirectorySectors(0)
		, numFatSectors(0)
		, firstDirectorySectorLocation(ENDOFCHAIN)
		, transactionSignatureNumber(0)
		, firstMiniFatSectorLocation(ENDOFCHAIN)
		, numMiniFatSectors(0)
		, firstDifatSectorLocation(ENDOFCHAIN)
		, numDifatSectors(0)
		, sectorSize(1u << sectorShift)
	{
		analyze(data, dataSize);
	}
	void addDifat(const char *data, size_t num)
	{
		for (uint32_t i = 0; i < num; i++) {
			uint32_t v = cybozu::Get32bitAsLE(data + i * 4);
			if (v == FREESECT) continue;
			difat.push_back(v);
		}
	}
	void analyze(const char *data, uint32_t dataSize)
	{
		if (data == 0) return;
		if (dataSize < 512) {
			throw cybozu::Exception("ms:cfb:Header:short dataSize") << dataSize;
		}
		if (memcmp(data, "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1", 8) != 0) {
			throw cybozu::Exception("ms:cfb:Header:invalid header signature");
		}
		if (!isZero(data + 8, 16)) {
			throw cybozu::Exception("ms:cfb:Header:invalid header CLSID");
		}
		uint32_t byteOrder, miniSectorShift, miniStreamCutoffSize;
		struct {
			int size;
			size_t pos;
			uint32_t *p;
		} tbl[] = {
			{ 16, 0x18, &minorVersion },
			{ 16, 0x1a, &majorVersion },
			{ 16, 0x1c, &byteOrder },
			{ 16, 0x1e, &sectorShift },
			{ 16, 0x20, &miniSectorShift },
			{ 32, 0x28, &numDirectorySectors },
			{ 32, 0x2c, &numFatSectors },
			{ 32, 0x30, &firstDirectorySectorLocation },
			{ 32, 0x34, &transactionSignatureNumber },
			{ 32, 0x38, &miniStreamCutoffSize },
			{ 32, 0x3c, &firstMiniFatSectorLocation },
			{ 32, 0x40, &numMiniFatSectors },
			{ 32, 0x44, &firstDifatSectorLocation },
			{ 32, 0x48, &numDifatSectors },
		};
		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
			const char *p = data + tbl[i].pos;
			*tbl[i].p = (tbl[i].size == 16) ? cybozu::Get16bitAsLE(p) : cybozu::Get32bitAsLE(p);
		}
		sectorSize = 1u << sectorShift;

		MS_ASSERT(minorVersion == 0x3eu || minorVersion == 0x3bu);
		MS_ASSERT(majorVersion == 3 || majorVersion == 4);
		MS_ASSERT_EQUAL(byteOrder, 0xfffeu);
		MS_ASSERT((majorVersion == 3 && sectorShift == 9) || (majorVersion == 4 && sectorShift == 0xc));
		MS_ASSERT_EQUAL(miniSectorShift, 6u);
		if (!isZero(data + 0x22, 6)) {
			throw cybozu::Exception("ms:cfb:Header:invalid reserved");
		}
		MS_ASSERT(majorVersion == 4 || numDirectorySectors == 0);
		MS_ASSERT_EQUAL(miniStreamCutoffSize, 0x1000u);
		addDifat(data + 0x4c, firstNumDIFAT);
		addExtraDifat(data, dataSize);
		put();
	}
	// input firstDifatSectorLocation, numDifatSectors
	void addExtraDifat(const char *data, uint32_t dataSize)
	{
		uint32_t pos = firstDifatSectorLocation;
		if (pos == ENDOFCHAIN) return;
		for (;;) {
			const char *p = data + 512 + pos * sectorSize;
			if (p >= data + dataSize) throw cybozu::Exception("ms:cfb:Header:addExtraDifat:large pos") << pos << dataSize;
			for (uint32_t i = 0; i < sectorSize / 4 - 1; i++) {
				uint32_t v = cybozu::Get32bitAsLE(p + i * 4);
				if (v == FREESECT) continue;
				difat.push_back(v);
			}
			uint32_t v = cybozu::Get32bitAsLE(p + sectorSize - 4);
			if (v == ENDOFCHAIN) {
				if (difat.size() != numFatSectors) {
					put();
					throw cybozu::Exception("ms:cfb:Header:addExtraDifat:bad numFatSectors") << difat.size() << numFatSectors;
				}
				return;
			}
			pos = v;
		}
	}
	/*
		data has 512 bytes
	*/
	void write(char *data) const
	{
		memcpy(data, "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1", 8); // magic number
		memset(data + 8, 0, 16); // CLSID
		memset(data + 0x22, 0, 6); // reserved
		const uint32_t byteOrder = 0xfffe;
		const uint32_t miniSectorShift = 6;
		const uint32_t miniStreamCutoffSize = 0x1000;
		struct {
			int size;
			size_t pos;
			uint32_t v;
		} tbl[] = {
			{ 16, 0x18, minorVersion },
			{ 16, 0x1a, majorVersion },
			{ 16, 0x1c, byteOrder },
			{ 16, 0x1e, sectorShift },
			{ 16, 0x20, miniSectorShift },
			{ 32, 0x28, numDirectorySectors },
			{ 32, 0x2c, numFatSectors },
			{ 32, 0x30, firstDirectorySectorLocation },
			{ 32, 0x34, transactionSignatureNumber },
			{ 32, 0x38, miniStreamCutoffSize },
			{ 32, 0x3c, firstMiniFatSectorLocation },
			{ 32, 0x40, numMiniFatSectors },
			{ 32, 0x44, firstDifatSectorLocation },
			{ 32, 0x48, numDifatSectors },
		};
		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
			char *p = data + tbl[i].pos;
			if (tbl[i].size == 16) {
				cybozu::Set16bitAsLE(p, static_cast<uint16_t>(tbl[i].v));
			} else {
				cybozu::Set32bitAsLE(p, tbl[i].v);
			}
		}
		const uint32_t difatSize = static_cast<uint32_t>(difat.size());
		for (uint32_t i = 0; i < std::min<uint32_t>(difatSize, firstNumDIFAT); i++) {
			cybozu::Set32bitAsLE(data + 0x4c + i * 4, difat[i]);
		}
		for (uint32_t i = difatSize; i < firstNumDIFAT; i++) {
			cybozu::Set32bitAsLE(data + 0x4c + i * 4, NOSTREAM);
		}
	}
	void put() const
	{
		if (!isDebug()) return;
		printf("version : major = %u, minor = 0x%04x\n", majorVersion, minorVersion);
		printf("sectorShift = %u\n", sectorShift);
		printf("numDirectorySectors = %u\n", numDirectorySectors);
		printf("numFatSectors = %u\n", numFatSectors);
		printf("firstDirectorySectorLocation = %u\n", firstDirectorySectorLocation);
		printf("transactionSignatureNumber = %u\n", transactionSignatureNumber);
		printf("firstMiniFatSectorLocation = %s\n", toStr(firstMiniFatSectorLocation).c_str());
		printf("numMiniFatSectors = %u\n", numMiniFatSectors);
		printf("firstDifatSectorLocation = %s\n", toStr(firstDifatSectorLocation).c_str());
		printf("numDifatSectors = %u\n", numDifatSectors);
		printf("difat size=%d\n", (int)difat.size());
		putCompact(difat);
	}
private:
	bool isZero(const char *data, uint32_t dataSize) const
	{
		for (uint32_t i = 0; i < dataSize; i++) {
			if (data[i]) return false;
		}
		return true;
	}
};

struct FatSectors {
	ChainMap chains;
	void makeFatIdx1(UintVec& fatIdx, const char *data, uint32_t sectorSize) const
	{
		for (size_t i = 0; i < sectorSize; i += 4) {
			uint32_t v = cybozu::Get32bitAsLE(data + i);
			fatIdx.push_back(v);
		}
	}
	void makeFatIdx(UintVec& fatIdx, const char *data, uint32_t sectorSize, const UintVec& difat) const
	{
		for (size_t i = 0; i < difat.size(); i++) {
			uint32_t fatId = difat[i];
			if (fatId == NOSTREAM) continue;
			// QQQ:check fatId size
			makeFatIdx1(fatIdx, data + fatId * sectorSize, sectorSize);
		}
	}
	/*
		@note destroy fatIdx
	*/
	void makeChains(ChainMap& chains, UintVec& fatIdx) const
	{
		for (uint32_t i = 0, n = (uint32_t)fatIdx.size(); i < n; i++) {
			uint32_t v = fatIdx[i];
			if (v != FREESECT && v != FATSECT && v != DIFSECT) {
 				UintVec& c = chains[i];
				c.push_back(i);
				while (v != ENDOFCHAIN) {
					if (v == NOSTREAM) break;
					if (v >= fatIdx.size()) {
						throw cybozu::Exception("ms:cfb:FatSectors:bad idx") << v;
					}
					c.push_back(v);
					uint32_t& next = fatIdx[v];
					v = next;
					next = FREESECT;
				}
			}
		}
	}
	void analyze(const char *data, uint32_t sectorSize, const UintVec& difat)
	{
		UintVec fatIdx;
		makeFatIdx(fatIdx, data, sectorSize, difat);
		makeChains(chains, fatIdx);
	}
	/*
		The mini FAT is stored in a standard sector chain of the FAT,
		so read all sectors in the chain. (see [MS-CFB] 2.4)
	*/
	void analyzeMini(const char *data, uint32_t sectorSize, const UintVec& miniFatSectors)
	{
		UintVec fatIdx;
		for (size_t i = 0; i < miniFatSectors.size(); i++) {
			makeFatIdx1(fatIdx, data + miniFatSectors[i] * sectorSize, sectorSize);
		}
		makeChains(chains, fatIdx);
	}
	void put() const
	{
		for (ChainMap::const_iterator i = chains.begin(), ie = chains.end(); i != ie; ++i) {
			putCompact(i->second);
		}
	}
	const UintVec* query(uint32_t id) const
	{
		ChainMap::const_iterator i = chains.find(id);
		if (i == chains.end()) return 0;
		return &(i->second);
	}
	const UintVec& get(uint32_t id) const
	{
		const UintVec *p = query(id);
		if (p == 0) {
			throw cybozu::Exception("ms:cfb:FatSectors:get") << id;
		}
		return *p;
	}
};

struct DirectoryEntry {
	struct FileTime {
		uint32_t dwLowDateTime;
		uint32_t dwHighDateTime;
		FileTime()
			: dwLowDateTime(0)
			, dwHighDateTime(0)
		{
		}
		std::string toString() const
		{
			if (dwLowDateTime == 0 && dwHighDateTime == 0) return "not set";
			cybozu::Time time;
			time.setByFILETIME(dwLowDateTime, dwHighDateTime);
			return time.toString();
		}
		void setCurrentTime()
		{
			cybozu::Time time;

		}
		void get(const char *data)
		{
			dwLowDateTime = cybozu::Get32bitAsLE(data);
			dwHighDateTime = cybozu::Get32bitAsLE(data + 4);
		}
		void set(char *data) const
		{
			cybozu::Set32bitAsLE(data, dwLowDateTime);
			cybozu::Set32bitAsLE(data + 4, dwHighDateTime);
		}
	};
	cybozu::String16 directoryEntryName;
	uint16_t directoryEntryNameLength;
	ObjectType objectType;
	ColorFlag colorFlag;
	uint32_t leftSiblingId;
	uint32_t rightSiblingId;
	uint32_t childId;
	std::string clsid;
	uint32_t stateBits;
	FileTime creationTime;
	FileTime modifiedTime;
	uint32_t startingSectorLocation;
	uint64_t streamSize;
	std::string content;

	// directoryEntryNameLength = directoryEntryName.size() * 2 + 2
	DirectoryEntry(const char *data = 0)
		: directoryEntryName()
		, directoryEntryNameLength(0)
		, objectType(Unallocated)
		, colorFlag(Red)
		, leftSiblingId(0)
		, rightSiblingId(0)
		, childId(0)
		, clsid()
		, stateBits(0)
		, creationTime()
		, modifiedTime()
		, startingSectorLocation(0)
		, streamSize(0)
	{
		if (data) analyze(data);
	}
	void analyze(const char *data)
	{
		directoryEntryNameLength = getDirectoryEntryNameLength(data + 0x40);

		directoryEntryName = getDirectoryEntryName(data, directoryEntryNameLength);
		objectType = getObjectType(data + 0x42);
		colorFlag = getColorFlag(data + 0x43);
		leftSiblingId = getId(data + 0x44);
		rightSiblingId = getId(data + 0x48);
		childId = getId(data + 0x4c);
		clsid.assign(reinterpret_cast<const char*>(data + 0x50), 16);
		stateBits = cybozu::Get32bitAsLE(data + 0x60);
		creationTime.get(data + 0x64);
		modifiedTime.get(data + 0x6c);
		startingSectorLocation = cybozu::Get32bitAsLE(data + 0x74);
		streamSize = cybozu::Get64bitAsLE(data + 0x78);
		if (streamSize >= uint64_t(0x100000000ULL)) {
			printf("warning too large size=%lld\n", (long long)streamSize);
			// clear upper 32bit if version 3
			streamSize &= 0xffffffff;
		}
	}
	void set(
		const cybozu::String16& name,
		ObjectType objectType,
		ColorFlag colorFlag,
		uint32_t leftSiblingId,
		uint32_t rightSiblingId,
		uint32_t childId,
		const std::string& clsid,
		uint32_t stateBits,
		const FileTime& creationTime,
		const FileTime& modifiedTime,
		uint32_t startingSectorLocation,
		const std::string& content)
	{
		setDirectoryEntryName(name);
		this->objectType = objectType;
		this->colorFlag = colorFlag;
		setId(this->leftSiblingId, leftSiblingId, "leftSiblingId");
		setId(this->rightSiblingId, rightSiblingId, "rightSiblingId");
		setId(this->childId, childId, "childId");
		if (!clsid.empty() && clsid.size() != 16) throw cybozu::Exception("ms:cfb:DirectoryEntry:bad clsid") << hex(clsid);
		this->stateBits = stateBits;
		this->creationTime = creationTime;
		this->modifiedTime = modifiedTime;
		this->startingSectorLocation = startingSectorLocation;
		this->streamSize = content.size();
		this->content = content;
	}
	/*
		write directory entry to out(128byte)
	*/
	void write(char *data) const
	{
		memset(data, 0, 64);
		memcpy(data, directoryEntryName.c_str(), (directoryEntryName.size() + 1) * 2);
		{
			uint16_t len = directoryEntryNameLength > 2 ? directoryEntryNameLength : 0;
			cybozu::Set16bitAsLE(data + 0x40, len);
		}
		data[0x42] = static_cast<char>(objectType);
		data[0x43] = static_cast<char>(colorFlag);
		cybozu::Set32bitAsLE(data + 0x44, leftSiblingId);
		cybozu::Set32bitAsLE(data + 0x48, rightSiblingId);
		cybozu::Set32bitAsLE(data + 0x4c, childId);
		if (clsid.empty()) {
			memset(data + 0x50, 0, 16);
		} else {
			MS_ASSERT(clsid.size() == 16);
			memcpy(data + 0x50, clsid.c_str(), 16);
		}
		cybozu::Set32bitAsLE(data + 0x60, stateBits);
		creationTime.set(data + 0x64);
		modifiedTime.set(data + 0x6c);
		cybozu::Set32bitAsLE(data + 0x74, startingSectorLocation);
		cybozu::Set64bitAsLE(data + 0x78, streamSize);
	}
	void writeContent(char *data, uint32_t sectorSize) const
	{
		const size_t size = content.size();
		if (size == 0) return;
		memcpy(data, &content[0], size);
		const size_t r = size % sectorSize;
		if (r > 0) {
			memset(data + size, 0, sectorSize - r);
		}
	}
	void put() const
	{
		if (!isDebug()) return;
		printf("directoryEntryName = ");
#ifdef _WIN32
		if (directoryEntryName.empty()) {
			printf("<none>");
		} else {
			printf("%S", &directoryEntryName[0]);
		}
#else
		printf("%s", toHex(reinterpret_cast<const char*>(&directoryEntryName[0]), directoryEntryNameLength).c_str());
#endif
		printf("(%u)\n", directoryEntryNameLength);
		printf("objectType = %d(%s)\n", objectType, toStr(objectType).c_str());
		printf("colorFlag = %d(%s)\n", colorFlag, toStr(colorFlag).c_str());
		printf("leftSiblingId = %s, ", toStr(leftSiblingId).c_str());
		printf("rightSiblingId = %s\n", toStr(rightSiblingId).c_str());
		printf("childId = %s\n", toStr(childId).c_str());
		printf("clsid = %s, ", toHex(clsid.c_str(), clsid.size()).c_str());
		printf("stateBits = 0x%08x\n", stateBits);
		printf("creation/modified Time = %s / %s\n", creationTime.toString().c_str(), modifiedTime.toString().c_str());
		printf("startingSectorLocation = %s\n", toStr(startingSectorLocation).c_str());
		printf("streamSize = %lld\n", (long long)streamSize);
		if (content.empty()) {
			printf("data=<empty>\n");
		} else {
			if (isDebug(2)) {
				std::string fileName = cybozu::ToUtf8(directoryEntryName) + ".dump";
				for (size_t i = 0; i < fileName.size(); i++) {
					char c = fileName[i];
					if (!isprint(c)) fileName[i] = '_';
				}
				saveFile(fileName, content);
			}
			if (streamSize <= 256) {
				printf("data=\n");
				dump16(content, content.size());
			} else {
				printf("data=[");
				for (size_t i = 0; i < 5; i++) {
					printf("%02x:", (uint8_t)content[i]);
				}
				printf(" ... ");
				for (size_t i = content.size() - 5; i < content.size(); i++) {
					printf("%02x:", (uint8_t)content[i]);
				}
				printf("]\n");
			}
		}
	}
private:
	uint16_t getDirectoryEntryNameLength(const char *data) const
	{
		uint16_t len = cybozu::Get16bitAsLE(data);
		MS_ASSERT(len <= 64);
		MS_ASSERT((len & 1) == 0);
		return len;
	}
	bool isValidEntryNameChar(uint16_t c) const
	{
		return c != 0 && c != '/' && c != ':' && c != '!';
	}
	bool isValidId(uint32_t id) const
	{
		return id <= MAXREGSECT || id == NOSTREAM;
	}
	uint32_t getId(const char *data) const
	{
		uint32_t id = cybozu::Get32bitAsLE(data);
		if (!isValidId(id)) throw cybozu::Exception("ms:cfb:DirectoryEntry:getId:bad id") << id;
		return id;
	}
	void setId(uint32_t& out, uint32_t id, const char *idName)
	{
		if (!isValidId(id)) throw cybozu::Exception("ms:cfb:DirectoryEntry:setId:bad") << idName << id;
		out = id;
	}
	cybozu::String16 getDirectoryEntryName(const char *data, uint16_t nameLength) const
	{
		cybozu::String16 str;
		if (nameLength == 0) return str;
		const uint16_t *p = reinterpret_cast<const uint16_t*>(data);
		const int len = (nameLength - 2) / 2;
		for (int i = 0; i < len; i++) {
			uint16_t c = p[i];
			if (!isValidEntryNameChar(c)) {
				throw cybozu::Exception("ms:cfb:DirectoryEntry:getDirectoryEntryName:bad char") << i << c;
			}
			str += c;
		}
		MS_ASSERT(p[len] == 0);
		return str;
	}
	void setDirectoryEntryName(const cybozu::String16& name)
	{
		const size_t len = name.size();
		if (len > 31) throw cybozu::Exception("ms:cfb:DirectoryEntry:setDirectoryEntryName:bad length") << len;
		directoryEntryNameLength = uint16_t((len + 1) * 2);
		for (size_t i = 0; i < len; i++) {
			uint16_t c = name[i];
			if (!isValidEntryNameChar(c)) {
				throw cybozu::Exception("ms:cfb:DirectoryEntry:setDirectoryEntryName:bad char") << i << c;
			}
		}
		directoryEntryName = name;
	}
	ObjectType getObjectType(const char *data) const
	{
		const char v = *data;
		MS_ASSERT(v == 0 || v == 1 || v == 2 || v == 5);
		return static_cast<ObjectType>(v);
	}
	ColorFlag getColorFlag(const char *data) const
	{
		const char v = *data;
		MS_ASSERT(v == 0 || v == 1);
		return static_cast<ColorFlag>(v);
	}
};

struct DirectoryEntryVec : std::vector<DirectoryEntry> {
	void analyze(const char *data, uint32_t sectorSize, const UintVec& chain)
	{
		for (size_t i = 0; i < chain.size(); i++) {
			const char *p = data + sectorSize * (chain[i] + 1);
			for (size_t j = 0; j < sectorSize; j += 128) {
				DirectoryEntry dir(p + j);
				if (!dir.directoryEntryName.empty()) {
					push_back(dir);
				}
			}
		}
	}
	void load(std::string& content, const char *data, uint64_t dataSize, const UintVec& chain, uint64_t blockSize)
	{
		for (size_t i = 0, n = chain.size(); i < n; i++) {
			if (content.size() >= dataSize) break;
			uint64_t pos = chain[i] * blockSize;
			const char *p = &data[pos];
			content.insert(content.end(), p, p + blockSize);
		}
		dprintf("dataSize=%d, content.size()=%d\n", (int)dataSize, (int)content.size());
		if (dataSize > content.size()) throw cybozu::Exception("ms:cfb:DirectoryEntryVec:short size") << dataSize << content.size();
		content.resize((size_t)dataSize);
	}
	void setContents(const char *data, uint32_t sectorSize, const FatSectors& fats, const FatSectors& miniFats)
	{
		data += sectorSize;
		std::string miniData;
		for (size_t i = 0; i < size(); i++) {
			DirectoryEntry& dir = (*this)[i];
			switch (dir.objectType) {
			case RootStorageObject:
				{
					const UintVec *v = fats.query(dir.startingSectorLocation);
					if (v) {
						load(miniData, data, dir.streamSize, *v, sectorSize);
					}
				}
				break;
			case StreamObject: // file
				{
					/*
						https://msdn.microsoft.com/en-us/library/dd941946.aspx
						Any user-defined data stream that is greater than or equal to this cutoff
						size must be allocated as normal sectors from the FAT.
					*/
					const uint64_t cutoffSize = 4096;
					const uint64_t miniSectorSize = 64;
					const uint32_t pos = dir.startingSectorLocation;

					const UintVec *v;
					if (dir.streamSize < cutoffSize && (v = miniFats.query(pos)) != 0) {
						load(dir.content, &miniData[0], dir.streamSize, *v, miniSectorSize);
					} else {
						load(dir.content, data, dir.streamSize, fats.get(pos), sectorSize);
					}
				}
				break;
			case StorageObject: // dir
				break;
			case Unallocated:
			default:
				break;
			}
		}
	}
	size_t getAllSectorNum(size_t sectorSize) const
	{
		size_t num = 0;
		for (size_t i = 0; i < size(); i++) {
			const DirectoryEntry& dir = (*this)[i];
			if (dir.objectType == StreamObject && dir.content.size() > 4096) {
				dprintf("size=%d\n", (int)dir.content.size());
				num += (dir.content.size() + sectorSize - 1) / sectorSize;
			}
		}
		return num;
	}
};

struct CompoundFile {
	Header header;
	FatSectors fats;
	FatSectors miniFats;
	DirectoryEntryVec dirs;
	explicit CompoundFile(const char *data = 0, uint32_t dataSize = 0)
		: header(data, dataSize)
	{
		if (data) analyze(data, dataSize);
	}
	void analyze(const char *data, uint32_t dataSize)
	{
		dprintf("dataSize=%lld(0x%llx)\n", (long long)dataSize, (long long)dataSize);
		const uint32_t version = header.majorVersion;
		MS_ASSERT_EQUAL(version, 3u);
		const uint32_t sectorSize = header.sectorSize;
		dprintf("sectorSize=%u\n", sectorSize);
		if (header.firstDirectorySectorLocation >= dataSize / sectorSize) {
			throw cybozu::Exception("ms:cfb:CompoundFile:analyze:large size") << header.firstDirectorySectorLocation;
		}
		fats.analyze(data + 512, sectorSize, header.difat);
		if (header.numMiniFatSectors > 0) {
			dprintf("# of mini fat sectors = %d\n", header.numMiniFatSectors);
			const UintVec& miniFatSectors = fats.get(header.firstMiniFatSectorLocation);
			if (miniFatSectors.size() != header.numMiniFatSectors) {
				throw cybozu::Exception("ms:cfb:CompoundFile:analyze:bad numMiniFatSectors") << miniFatSectors.size() << header.numMiniFatSectors;
			}
			for (size_t i = 0; i < miniFatSectors.size(); i++) {
				if (miniFatSectors[i] + 1 >= dataSize / sectorSize) {
					throw cybozu::Exception("ms:cfb:CompoundFile:analyze:bad miniFatSectorLocation") << miniFatSectors[i];
				}
			}
			miniFats.analyzeMini(data + 512, sectorSize, miniFatSectors);
		}
		dprintf("analyze dirs\n");
		dirs.analyze(data, sectorSize, fats.chains[header.firstDirectorySectorLocation]);
		dprintf("load contents\n");
		dirs.setContents(data, sectorSize, fats, miniFats);
	}
	void put() const
	{
		if (!isDebug()) return;
		header.put();
		printf("FatSectors\n");
		fats.put();
		printf("mini FatSectors\n");
		miniFats.put();
		for (size_t i = 0; i < dirs.size(); i++) {
			printf("----------------------------\n");
			printf("DirectoryEntry %lld\n", (long long)i);
			dirs[i].put();
		}
	}
};

} } // ms::cfb

