#pragma once
/**
	@file
	@brief line stream class
*/
#include <string>
#include <memory.h>
#include <iosfwd>
#include <cybozu/file.hpp>

namespace cybozu {

namespace line_stream {

#define CYBOZU_STREAM_CRLF "\x0d\x0a"

const char CR = '\x0d';
const char LF = '\x0a';
const char CRLF[] = CYBOZU_STREAM_CRLF;

} // line_stream

/**
	construct lines from reader
	accept 0x0d 0xa and 0x0a and remove them
	next() returns one line without CRLF
	throw exception if line.size() > maxLineSize (line.size() does not include CRLF)

	Remark:next() may return the last data without CRLF

	Reader must have size_t readSome(char *buf, size_t size); method

	How to use this

	LinstStreamT<Reader> lineStream(reader);
	for (;;) {
		std::string line;
		if (!lineStream.next(line)) break;
	}
*/
template<class Reader>
class LineStreamT {
	Reader& reader_;
	size_t maxLineSize_;
	std::string buf_;
	size_t bufSize_;
	const char *next_;
	bool eof_;
	LineStreamT(const LineStreamT&);
	void operator=(const LineStreamT&);
	void verifySize(const char *begin, const char *end) const
	{
		if (static_cast<uintptr_t>(end - begin) > maxLineSize_) {
			throw cybozu::Exception("LineStreamT:next:line is too long") << cybozu::exception::makeString(begin, 10);
		}
	}
public:
	explicit LineStreamT(Reader& reader, size_t maxLineSize = 1024 * 2)
		: reader_(reader)
		, maxLineSize_(maxLineSize)
		, buf_(maxLineSize * 2, 0)
		, bufSize_(0)
		, next_(buf_.data())
		, eof_(false)
	{
	}
	/**
		get line without CRLF
		@param begin [out] begin of line
		@param end [out] end of line
		@retval true if sucess
		@retval false if not data
	*/
	bool next(const char **begin, const char **end)
	{
		if (eof_) return false;
		for (;;) {
			const size_t remainSize = &buf_[bufSize_] - next_;
			if (remainSize > 0) {
				const char *endl = static_cast<const char *>(memchr(next_, cybozu::line_stream::LF, remainSize));
				if (endl) {
					if (endl > next_ && endl[-1] == cybozu::line_stream::CR) {
						*end = endl - 1;
					} else {
						*end = endl;
					}
					*begin = next_;
					verifySize(*begin, *end);
					next_ = endl + 1;
					return true;
				}
				// move next_ to top of buf_
				for (size_t i = 0; i < remainSize; i++) {
					buf_[i] = next_[i];
				}
				bufSize_ = remainSize;
			} else {
				bufSize_ = 0;
			}
			next_ = buf_.data();
			size_t readSize = reader_.readSome(&buf_[bufSize_], buf_.size() - bufSize_);
			if (readSize == 0) {
				eof_ = true;
				if (bufSize_ == 0) return false;
				if (bufSize_ > maxLineSize_) {
					throw cybozu::Exception("LineStreamT:next:no CRLF");
				}
				// take all remain buffer
				*begin= buf_.data();
				*end = &buf_[bufSize_];
				verifySize(*begin, *end);
				return true;
			}
			bufSize_ += readSize;
		}
	}
	/**
		get line
	*/
	bool next(std::string& line)
	{
		const char *begin;
		const char *end;
		if (next(&begin, &end)) {
			line.assign(begin, end);
			return true;
		}
		return false;
	}
	/*
		get remaining raw data
	*/
	void getRemain(std::string& remain) const
	{
		if (eof_) {
			remain.clear();
		} else {
			remain.assign(next_, &buf_[bufSize_]);
		}
	}
	std::string getRemain() const
	{
		std::string remain;
		getRemain(remain);
		return remain;
	}
};

} // cybozu
