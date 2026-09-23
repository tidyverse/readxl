#pragma once
/**
	@file
	@brief unicode string class like std::string
	support char*, std::string with UTF-8

	@author MITSUNARI Shigeo(@herumi)
*/

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4702)
#endif

#include <string>
#include <cstring>
#include <cwchar> // for mbstate_t, used by the char_traits<unsigned int> specialization below
#include <assert.h>
#include <stddef.h>
#include <stdio.h> // for printf
#include <iosfwd> // for istream, ostream
#include <functional> // for unary_function

#include <cybozu/exception.hpp>
#include <cybozu/hash.hpp>

#if CYBOZU_CPLUSPLUS >= 202002L
#include <compare>
#endif


// to deal with unicode literal(same macro defined in regex.hpp)
#ifdef _MSC_VER
	#define CYBOZU_STR_WCHAR wchar_t
	#define CYBOZU_STR_W(x) L##x // assume UTF-16 string
#else
	#define CYBOZU_STR_WCHAR char
	#define CYBOZU_STR_W(x) x // assume UTF-8 string
#endif

#if defined(_LIBCPP_VERSION) && !(defined(__GNUC__) && (__SIZEOF_WCHAR_T__ == 4))
// cybozu::Char is `unsigned int` on this platform (wchar_t is only 16 bits,
// see below), so std::basic_string<unsigned int> needs an explicit
// std::char_traits<unsigned int>. libstdc++ and MSVC STL happen to provide a
// non-standard generic fallback body for the primary char_traits template
// that silently covers this, but libc++ (used by llvm-mingw) leaves
// the primary template undefined for any CharT it hasn't specialized
// itself, which is a hard compile error the moment it's instantiated.
// Define it only for libc++: MSVC STL's basic_string calls internal members
// of char_traits (_Copy_s, _Move_s, ...) that a standard-conforming
// specialization does not have, so replacing its primary template breaks
// the build.
namespace std {

template<>
struct char_traits<unsigned int> {
	typedef unsigned int char_type;
	typedef long long int_type;
	typedef std::streamoff off_type;
	typedef std::streampos pos_type;
	typedef mbstate_t state_type;

	static void assign(char_type& c1, const char_type& c2) CYBOZU_NOEXCEPT { c1 = c2; }
	static bool eq(char_type c1, char_type c2) CYBOZU_NOEXCEPT { return c1 == c2; }
	static bool lt(char_type c1, char_type c2) CYBOZU_NOEXCEPT { return c1 < c2; }

	static int compare(const char_type* s1, const char_type* s2, size_t n)
	{
		for (size_t i = 0; i < n; i++) {
			if (lt(s1[i], s2[i])) return -1;
			if (lt(s2[i], s1[i])) return 1;
		}
		return 0;
	}
	static size_t length(const char_type* s)
	{
		size_t n = 0;
		while (!eq(s[n], char_type())) n++;
		return n;
	}
	static const char_type* find(const char_type* s, size_t n, const char_type& a)
	{
		for (size_t i = 0; i < n; i++) {
			if (eq(s[i], a)) return s + i;
		}
		return 0;
	}
	static char_type* move(char_type* s1, const char_type* s2, size_t n)
	{
		if (n == 0) return s1;
		return static_cast<char_type*>(memmove(s1, s2, n * sizeof(char_type)));
	}
	static char_type* copy(char_type* s1, const char_type* s2, size_t n)
	{
		if (n == 0) return s1;
		return static_cast<char_type*>(memcpy(s1, s2, n * sizeof(char_type)));
	}
	static char_type* assign(char_type* s, size_t n, char_type a)
	{
		for (size_t i = 0; i < n; i++) s[i] = a;
		return s;
	}
	static int_type not_eof(int_type c) CYBOZU_NOEXCEPT { return eq_int_type(c, eof()) ? 0 : c; }
	static char_type to_char_type(int_type c) CYBOZU_NOEXCEPT { return static_cast<char_type>(c); }
	static int_type to_int_type(char_type c) CYBOZU_NOEXCEPT { return static_cast<int_type>(c); }
	static bool eq_int_type(int_type c1, int_type c2) CYBOZU_NOEXCEPT { return c1 == c2; }
	static int_type eof() CYBOZU_NOEXCEPT { return static_cast<int_type>(-1); }
};

} // std
#endif

namespace cybozu {

#if defined(__GNUC__) && (__SIZEOF_WCHAR_T__ == 4)
	/* avoid to use uint32_t because compiling boost::regex fails */
	typedef wchar_t Char; //!< Char for Linux
#if __cplusplus >= 201103L
	typedef char16_t Char16;
#else
	typedef unsigned short Char16; /* unsigned is necessary for gcc */
#endif
#else
	typedef unsigned int Char; //!< Char for Windows
	typedef wchar_t Char16;
#endif

typedef std::basic_string<Char16> String16;

template<class CharT>
size_t strlen(const CharT *str)
{
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

template<class CharT>
CharT tolower(CharT c)
{
	return ('A' <= c && c <= 'Z') ? c - 'A' + 'a' : c;
}

template<class CharT>
CharT toupper(CharT c)
{
	return ('a' <= c && c <= 'z') ? c - 'a' + 'A' : c;
}

namespace string_local {

/* true if c in [min, max] */
inline bool in(unsigned char c, int min, int max)
{
//	  return min <= c && c <= max;
	return static_cast<unsigned int>(c - min) <= static_cast<unsigned int>(max - min);
}

template<class T>
struct IsInt { enum { value = false }; };
template<>struct IsInt<int> { enum { value = true }; };
template<>struct IsInt<unsigned int> { enum { value = true }; };
template<>struct IsInt<long> { enum { value = true }; };
template<>struct IsInt<unsigned long> { enum { value = true }; };
template<>struct IsInt<long long> { enum { value = true }; };
template<>struct IsInt<unsigned long long> { enum { value = true }; };

template <bool b, class T = void>
struct disable_if { typedef T type; };

template <class T>
struct disable_if<true, T> {};

} // string_local

/**
	utility function
*/
namespace string {

/*
	 code point[a, b] 1byte  2ybte  3byte  4byte
	  U+0000   U+007f 00..7f                      ; 128
	  U+0080   U+07ff c2..df 80..bf               ; 30 x 64 = 1920

	  U+0800   U+0fff e0     a0..bf 80..bf        ;  1 x 32 x 64 = 2048
	  U+1000   U+cfff e1..ec 80..bf 80..bf        ; 12 x 64 x 64 = 49152
	  U+d000   U+d7ff ed     80..9f 80..bf        ;  1 x 32 x 64 = 2048

	  U+e000   U+ffff ee..ef 80..bf 80..bf        ;  2 x 64 x 64 = 8192

	 U+10000  U+3ffff f0     90..bf 80..bf 80..bf ;  1 x 48 x 64 x 64 = 196608
	 U+40000  U+fffff f1..f3 80..bf 80..bf 80..bf ;  3 x 64 x 64 x 64 = 786432
	U+100000 U+10ffff f4     80..8f 80..bf 80..bf ;  1 x 16 x 64 x 64 = 65536
*/
inline int GetCharSize(Char c)
{
	if (c <= 0x7f) return 1;
	if (c <= 0x7ff) return 2;
	if (c <= 0xd7ff) return 3;
	if (c <= 0xdfff || c > 0x10ffff) return 0;
	if (c <= 0xffff) return 3;
	return 4;
}

// for Char/char
inline bool IsValidChar(Char c)
{
	return GetCharSize(c) != 0;
}

/*
	get one character from UTF-8 string and seek begin to next char
	@note begin != end
	@note begin is not determined if false
*/
template<class Iterator>
bool GetCharFromUtf8(Char *c, Iterator& begin, const Iterator& end)
{
	unsigned char c0 = *begin++;
	if (c0 <= 0x7f) {
		*c = c0;
		return true;
	}
	if (string_local::in(c0, 0xc2, 0xdf)) {
		if (begin != end) {
			unsigned char c1 = *begin++;
			if (string_local::in(c1, 0x80, 0xbf)) {
				*c = ((c0 << 6) | (c1 & 0x3f)) - 0x3000;
				return true;
			}
		}
	} else if (c0 <= 0xef) {
		if (begin != end) {
			unsigned char c1 = *begin++;
			if (begin != end) {
				unsigned char c2 = *begin++;
				if (string_local::in(c2, 0x80, 0xbf)) {
					if ((c0 == 0xe0 && string_local::in(c1, 0xa0, 0xbf))
					 || (string_local::in(c0, 0xe1, 0xec) && string_local::in(c1, 0x80, 0xbf))
					 || (c0 == 0xed && string_local::in(c1, 0x80, 0x9f))
					 || (string_local::in(c0, 0xee, 0xef) && string_local::in(c1, 0x80, 0xbf))) {
						*c = ((c0 << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f)) - 0xe0000;
						return true;
					}
				}
			}
		}
	} else if (string_local::in(c0, 0xf0, 0xf4)) {
		if (begin != end) {
			unsigned char c1 = *begin++;
			if (begin != end) {
				unsigned char c2 = *begin++;
				if (begin != end) {
					unsigned char c3 = *begin++;
					if (string_local::in(c2, 0x80, 0xbf) && string_local::in(c3, 0x80, 0xbf)) {
						if ((c0 == 0xf0 && string_local::in(c1, 0x90, 0xbf))
						 || (string_local::in(c0, 0xf1, 0xf3) && string_local::in(c1, 0x80, 0xbf))
						 || (c0 == 0xf4 && string_local::in(c1, 0x80, 0x8f))) {
							*c = ((c0 << 18) | ((c1 & 0x3f) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f)) - 0x3c00000;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/*
	get one character from UTF-16 string and seek begin to next char
	@note begin != end
	@note begin is not determined if false
*/
template<class Iterator>
bool GetCharFromUtf16(Char& c, Iterator& begin, const Iterator& end)
{
	struct string_local {
		static inline bool isLead(Char c) { return (c & 0xfffffc00) == 0xd800; }
		static inline bool isTrail(Char c) { return (c & 0xfffffc00) == 0xdc00; }
	};
	Char16 c0 = *begin++;
	if (!string_local::isLead(c0)) {
		c = c0;
		return true;
	}
	if (begin != end) {
		Char16 c1 = *begin++;
		if (string_local::isTrail(c1)) {
			const Char offset = (0xd800 << 10UL) + 0xdc00 - 0x10000;
			c = (c0 << 10) + c1 - offset;
			return true;
		}
	}
	return false;
}

inline int toUtf8(char out[4], Char c)
{
	if (c <= 0x7f) {
		out[0] = static_cast<char>(c);
		return 1;
	} else if (c <= 0x7ff) {
		out[0] = static_cast<char>((c >> 6) | 0xc0);
		out[1] = static_cast<char>((c & 0x3f) | 0x80);
		return 2;
	} else if (c <= 0xffff) {
		if (0xd7ff < c && c <= 0xdfff) {
			return 0;
		}
		out[0] = static_cast<char>((c >> 12) | 0xe0);
		out[1] = static_cast<char>(((c >> 6) & 0x3f) | 0x80);
		out[2] = static_cast<char>((c & 0x3f) | 0x80);
		return 3;
	} else if (c <= 0x10ffff) {
		out[0] = static_cast<char>((c >> 18) | 0xf0);
		out[1] = static_cast<char>(((c >> 12) & 0x3f) | 0x80);
		out[2] = static_cast<char>(((c >> 6) & 0x3f) | 0x80);
		out[3] = static_cast<char>((c & 0x3f) | 0x80);
		return 4;
	}
	return 0;
}

inline int toUtf16(Char16 out[2], Char c)
{
	if (c <= 0xffff) {
		out[0] = static_cast<Char16>(c);
		return 1;
	} else if (c <= 0x0010ffff) {
		out[0] = static_cast<Char16>((c >> 10) + 0xd7c0);
		out[1] = static_cast<Char16>((c & 0x3ff) | 0xdc00);
		return 2;
	}
	return 0;
}

inline bool AppendUtf8(std::string& out, Char c)
{
	char buf[4];
	int len = toUtf8(buf, c);
	if (len > 0) {
		out.append(buf, len);
		return true;
	}
	return false;
}

inline bool AppendUtf16(String16& out, Char c)
{
	Char16 buf[2];
	int len = toUtf16(buf, c);
	if (len > 0) {
		out.append(buf, len);
		return true;
	}
	return false;
}

} // string

template<class CharT, class Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
class StringT {
public:
	typedef std::basic_string<CharT, Traits, Alloc> BasicString;
	typedef CharT value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef CharT& reference;
	typedef const CharT& const_reference;
	typedef CharT* pointer;
	typedef const CharT* const_pointer;
	typedef typename BasicString::iterator iterator;
	typedef typename BasicString::const_iterator const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	//@}
	static const size_t npos = size_t(-1); //!< standard npos

	/**
		dump unicode of string for debug
		@param msg [in] user message
	*/
	void dump(const char *msg = 0) const
	{
		if (msg) printf("%s", msg);
		for (size_t i = 0; i < size(); i++) {
			printf("%08x ", str_[i]);
		}
		printf("\n");
	}

	/**
		construct empty string
	*/
	StringT() { }

	/**
		construct from str [off, off + count)
		@param str [in] original string
		@param off [in] offset
		@param count [in] count of character(default npos)
	*/
	StringT(const StringT& str, size_type off, size_type count = npos)
		: str_(str.str_, off, count)
	{ }

	/**
		construct from [str, str + count)
		@param str [in] original string
		@param count [in] count of character
	*/
	StringT(const CharT *str, size_type count)
		: str_(str, count)
	{
	}

	/**
		construct from [str, NUL)
		@param str [in] original string
	*/
	StringT(const CharT *str)
		: str_(str)
	{
	}

	/**
		construct from count * c
		@param count [in] count of character
		@param c [in] initial character
	*/
	StringT(size_type count, CharT c)
		: str_(count, c)
	{
	}

	/**
		construct from [begin, end)
		@param begin [in] begin of iterator
		@param end [in] end of iterator
	*/
	template<class Iterator>
	StringT(Iterator begin, Iterator end, typename string_local::disable_if<string_local::IsInt<Iterator>::value>::type* = 0)
	{
		append(begin, end);
	}

	// construct from [begin, end), const pointers
//  StringT(const_pointer begin, const_pointer end);
	// construct from [begin, end), const_iterators
//  StringT(const_iterator begin, const_iterator end);

	/**
		construct by copying str
		@param str [in] original string
	*/
	StringT(const StringT& str)
		: str_(str.str_)
	{
	}

	/**
		construct by [str, str + count)
		@param str [in] original string
		@param count [in] count of character
	*/
	StringT(const char *str, size_type count) // A
	{
		append(str, count);
	}

	/**
		construct from [str, NUL)
		@param str [in] original string
	*/
	StringT(const char *str) // A
	{
		append(str);
	}
	/**
		construct by copying str
		@param str [in] original string
	*/
	StringT(const std::string& str) // A
	{
		append(str);
	}

	/**
		construt by Char16(same ICU::UChar)
		@param str [in] UTF-16 format string
	*/
	StringT(const String16& str) // A
	{
		append(&str[0], &str[0] + str.size());
	}
	StringT(const Char16* str) // A
	{
		append(str, str + cybozu::strlen(str));
	}
	/**
		construct by BasicString
		@param str [in] UTF-32 string
	*/
	StringT(const BasicString& str) // A
		: str_(str)
	{
	}

	/**
		assign str
		@param str [in] assign string
	*/
	StringT& operator=(const StringT& str)
	{
		return assign(str);
	}

	/**
		assign [str, NUL)
		@param str [in] assign string
	*/
	StringT& operator=(const CharT *str)
	{
		return assign(str);
	}

	/**
		assign 1 * c
		@param c [in] initial character
	*/
	StringT& operator=(CharT c)
	{
		return assign(1, c);
	}

	/**
		assign [str, NUL)
		@param str [in] assign string
	*/
	StringT& operator=(const char *str) // A
	{
		return assign(str);
	}
	/**
		assign str
		@param str [in] assign string
	*/
	StringT& operator=(const std::string& str) // A
	{
		return assign(str);
	}

	/**
		append str
		@param str [in] append string
	*/
	StringT& operator+=(const StringT& str)
	{
		return append(str);
	}

	/**
		append [str, NUL)
		@param str [in] append string
	*/
	StringT& operator+=(const CharT *str)
	{
		return append(str);
	}

	/**
		append 1 * c
		@param c [in] append character
	*/
	StringT& operator+=(CharT c)
	{
		return append(1, c);
	}

	/**
		append str
		@param str [in] append string
	*/
	StringT& append(const StringT& str)
	{
		str_.append(str.str_); return *this;
	}

	/**
		append str [off, off + count)
		@param str [in] append string
		@param off [in] string offset
		@param count [in] count of character
	*/
	StringT& append(const StringT& str, size_type off, size_type count)
	{
		str_.append(str.str_, off, count); return *this;
	}

	/**
		append [str, str + count)
		@param str [in] append string
		@param count [in] count of character
	*/
	StringT& append(const CharT *str, size_type count)
	{
		return append(str, str + count);
	}

	/**
		append [str, NUL)
		@param str [in] append string
	*/
	StringT& append(const CharT *str)
	{
		str_.append(str); return *this;
	}

	/**
		append count * c
		@param count [in] count of character
		@param c [in] initial character
	*/
	StringT& append(size_type count, CharT c)
	{
		str_.append(count, c); return *this;
	}

	/**
		append [begin, end)
		@param begin [in] begin of iterator
		@param end [in] end of iterator
	*/
	template<class Iterator>
	StringT& append(Iterator begin, Iterator end, typename string_local::disable_if<string_local::IsInt<Iterator>::value>::type* = 0)
	{
		while (begin != end) {
			CharT c;
			c = getOneChar(begin, end);
			str_.push_back(c);
		}
		return *this;
	}

	// append [begin, end), const pointers
//  StringT& append(const_pointer begin, const_pointer end);
	// append [begin, end), const_iterators
//  StringT& append(const_iterator begin, const_iterator end);

	/**
		append [str, str + count)
		@param str [in] append string
		@param count [in] count of character
	*/
	StringT& append(const char *str, size_type count) // A
	{
		return append(str, str + count);
	}

	/**
		append [str, NUL)
		@param str [in] append string
	*/
	StringT& append(const char *str) // A
	{
		return append(str, std::strlen(str));
	}
	StringT& append(const Char16 *begin, const Char16 *end)
	{
		while (begin != end) {
			Char c;
			if (!string::GetCharFromUtf16(c, begin, end)) {
				throw cybozu::Exception("StringT:GetCharFromUtf16");
			}
			str_ += c;
		}
		return *this;
	}
	StringT& append(const Char16 *str)
	{
		return append(str, str + cybozu::strlen(str));
	}
	/**
		append str
		@param str [in] append string
	*/
	StringT& append(const std::string& str) // A
	{
		return append(str.begin(), str.end());
	}

	/**
		assign str
		@param str [in] assign str
	*/
	StringT& assign(const StringT& str)
	{
		clear(); return append(str);
	}

	/**
		assign str [off, off + count)
		@param str [in] assign string
		@param off [in] offset
		@param count [in] count of character
	*/
	StringT& assign(const StringT& str, size_type off, size_type count)
	{
		clear(); return append(str, off, count);
	}

	/**
		assign [str, str + count)
		@param str [in] assign string
		@param count [in] count of character
	*/
	StringT& assign(const CharT *str, size_type count)
	{
		return assign(str, str + count);
	}

	/**
		assign [str, NUL)
		@param str [in] assign string
	*/
	StringT& assign(const CharT *str)
	{
		clear(); return append(str);
	}
	StringT& assign(const Char16 *str)
	{
		clear(); return append(str);
	}

	/**
		assign count * c
		@param count [in] count of character
		@param c [in] initial character
	*/
	StringT& assign(size_type count, CharT c)
	{
		clear(); return append(count, c);
	}

	/**
		assign [First, end)
		@param begin [in] begin of iterator
		@param end [in] end of iterator
	*/
	template<class Iterator>
	StringT& assign(Iterator begin, Iterator end, typename string_local::disable_if<string_local::IsInt<Iterator>::value>::type* = 0)
	{
		clear(); return append(begin, end);
	}

	// assign [First, end), const pointers
//  StringT& assign(const_pointer begin, const_pointer end);

	// assign [First, end), const_iterators
//  StringT& assign(const_iterator begin, const_iterator end);

	/**
		assign [str, str + count)
		@param str [in] original string
		@param count [in] count of character
	*/
	StringT& assign(const char *str, size_type count) // A
	{
		return assign(str, str + count);
	}

	/**
		assign [str, NUL)
		@param str [in] original string
	*/
	StringT& assign(const char *str) // A
	{
		clear(); return append(str);
	}
	/**
		assign str
		@param str [in] original string
	*/
	StringT& assign(const std::string& str) // A
	{
		clear(); return append(str);
	}

	/**
		insert str at off
		@param off [in] offset
		@param str [in] insert str
	*/
	StringT& insert(size_type off, const StringT& str)
	{
		str_.insert(off, str.str_); return *this;
	}

	/**
		insert str [off, off + count) at off
		@param off [in] offset of destination
		@param rhs [in] source str
		@param rhsOff [in] offset of source str
		@param count [in] count of source str
	*/
	StringT& insert(size_type off, const StringT& rhs, size_type rhsOff, size_type count)
	{
		str_.insert(off, rhs.str_, rhsOff, count); return *this;
	}

	/**
		insert [str, str + count) at off
		@param off [in] offset of destination
		@param str [in] source str
		@param count [in] count of source str
	*/
	StringT& insert(size_type off, const CharT *str, size_type count)
	{
		str_.insert(off, str, count); return *this;
	}

	/**
		insert [str, NUL) at off
		@param off [in] offset of destination
		@param str [in] source str
	*/
	StringT& insert(size_type off, const CharT *str)
	{
		str_.insert(off, str); return *this;
	}

	/**
		insert count * c at off
		@param off [in] offset of destination
		@param count [in] count of source str
		@param c [in] initial character
	*/
	StringT& insert(size_type off, size_type count, CharT c)
	{
		str_.insert(off, count, c); return *this;
	}
	/**
		insert c at here
		@param here [in] offset of destination
		@param c [in] initial character(default 0)
	*/
	iterator insert(iterator here, CharT c = 0)
	{
		return str_.insert(here, c);
	}

	/**
		insert count * CharT at here
		@param here [in] offset of destination
		@param count [in] count of str
		@param c [in] initial character
	*/
	void insert(iterator here, size_type count, CharT c)
	{
		str_.insert(here, count, c);
	}

	/**
		insert [begin, end) at here
		@param here [in] offset of destination
		@param begin [in] begin of iterator
		@param end [in] end of iterator
	*/
	template<class Iterator>
	void insert(iterator here, Iterator begin, Iterator end)
	{
		StringT str(begin, end);
		str_.insert(here, str.begin(), str.end());
	}

	// insert [begin, end) at here, const pointers
//  void insert(iterator here, const_pointer begin, const_pointer end);
	// insert [begin, end) at here, const_iterators
//  void insert(iterator here, const_iterator begin, const_iterator end);

	/**
		erase elements [off, off + count)
		@param off [in] offset
		@param count [in] count of character(default npos)
	*/
	StringT& erase(size_type off = 0, size_type count = npos)
	{
		str_.erase(off, count); return *this;
	}

	/**
		erase element at here
		@param here [in] erase from here
	*/
	iterator erase(iterator here)
	{
		return str_.erase(here);
	}

	/**
		erase substring [begin, end)
		@param begin [in] begin of iterator
		@param end [in] end of iterator
	*/
	iterator erase(iterator begin, iterator end)
	{
		return str_.erase(begin, end);
	}

	/**
		erase all
	*/
	void clear() { str_.clear(); }

	/**
		replace [off, off + n) with rhs
		@param off [in] start offset
		@param n [in] count of remove character
		@param rhs [in] append string
	*/
	StringT& replace(size_type off, size_type n, const StringT& rhs)
	{
		str_.replace(off, n, rhs.str_); return *this;
	}

	/**
		replace [off, off + n) with rhs [rhsOff, rhsOff + count)
		@param off [in] start offset
		@param n [in] count of remove character
		@param rhs [in] append string
		@param rhsOff [in] append from
		@param count [in] count of append
	*/
	StringT& replace(size_type off, size_type n, const StringT& rhs, size_type rhsOff, size_type count)
	{
		str_.replace(off, n, rhs.str_, rhsOff, count); return *this;
	}

	/**
		replace [off, off + n) with [str, str + count)
		@param off [in] start offset
		@param n [in] count of remove character
		@param str [in] append string
		@param count [in] count of append
	*/
	StringT& replace(size_type off, size_type n, const CharT *str, size_type count)
	{
		str_.replace(off, n, str, count); return *this;
	}

	/**
		replace [off, off + n) with [str, NUL)
		@param off [in] start offset
		@param n [in] count of remove character
		@param str [in] append string
	*/
	StringT& replace(size_type off, size_type n, const CharT *str)
	{
		str_.replace(off, n, str); return *this;
	}

	/**
		replace [off, off + n) with count * c
		@param off [in] start offset
		@param n [in] count of remove character
		@param count [in] count of append
		@param c [in] initial character
	*/
	StringT& replace(size_type off, size_type n, size_type count, CharT c)
	{
		str_.replace(off, n, count, c); return *this;
	}

	/**
		replace [begin, end) with rhs
		@param begin [in] begin to remove
		@param end [in] end to remove
		@param rhs [in] append str
	*/
	StringT& replace(iterator begin, iterator end, const StringT& rhs)
	{
		str_.replace(begin, end, rhs.str_); return *this;
	}

	/**
		replace [begin, end) with [str, str + count)
		@param begin [in] begin to remove
		@param end [in] end to remove
		@param str in] append str
		@param count [in] count of append
	*/
	StringT& replace(iterator begin, iterator end, const CharT *str, size_type count)
	{
		str_.replace(begin, end, str, count); return *this;
	}

	/**
		replace [begin, end) with [str, NUL)
		@param begin [in] begin to remove
		@param end [in] end to remove
		@param str in] append str
	*/
	StringT& replace(iterator begin, iterator end, const CharT *str)
	{
		str_.replace(begin, end, str); return *this;
	}

	/**
		replace [begin, end) with count * c
		@param begin [in] begin to remove
		@param end [in] end to remove
		@param count [in] count of append
		@param c [in] initial character
	*/
	StringT& replace(iterator begin, iterator end, size_type count, CharT c)
	{
		str_.replace(begin, end, count, c); return *this;
	}

	/**
		replace [begin, end) with [begin2, end2)
		@param begin [in] begin to remove
		@param end [in] end to remove
		@param begin2 [in] begin to append
		@param end2 [in] end to append
	*/
	template<class Iterator>
	StringT& replace(iterator begin, iterator end, Iterator begin2, Iterator end2)
	{
		StringT str(begin2, end2);
		str_.replace(begin, end, str.begin(), str.end());
		return *this;
	}

	// replace [begin, end) with [begin2, end2), const pointers
//  StringT& replace(iterator begin, iterator end, const_pointer begin2, const_pointer end2);

	// replace [begin, end) with [begin2, end2), const_iterators
//  StringT& replace(iterator begin, iterator end, const_iterator begin2, const_iterator end2);

	/**
		return iterator for beginning of mutable sequence
	*/
	iterator begin() CYBOZU_NOEXCEPT { return str_.begin(); }

	/**
		return iterator for beginning of nonmutable sequence
	*/
	const_iterator begin() const CYBOZU_NOEXCEPT { return str_.begin(); }

	/**
		return iterator for end of mutable sequence
	*/
	iterator end() CYBOZU_NOEXCEPT { return str_.end(); }

	/**
		return iterator for end of nonmutable sequence
	*/
	const_iterator end() const CYBOZU_NOEXCEPT { return str_.end(); }

	/**
		return iterator for beginning of reversed mutable sequence
	*/
	reverse_iterator rbegin() CYBOZU_NOEXCEPT { return str_.rbegin(); }

	/**
		return iterator for beginning of reversed nonmutable sequence
	*/
	const_reverse_iterator rbegin() const CYBOZU_NOEXCEPT { return str_.rbegin(); }

	/**
		return iterator for end of reversed mutable sequence
	*/
	reverse_iterator rend() CYBOZU_NOEXCEPT { return str_.rend(); }

	/**
		return iterator for end of reversed nonmutable sequence
	*/
	const_reverse_iterator rend() const CYBOZU_NOEXCEPT { return str_.rend(); }

	/**
		subscript mutable sequence with checking
		@param off [in] offset
	*/
	reference at(size_type off) { return str_.at(off); }

	/**
		get element at off
		@param off [in] offset
	*/
	const_reference at(size_type off) const { return str_.at(off); }

	/**
		subscript mutable sequence
		@param off [in] offset
	*/
	reference operator[](size_type off) { return str_[off]; }

	/**
		subscript nonmutable sequence
		@param off [in] offset
	*/
	const_reference operator[](size_type off) const { return str_[off]; }

	/**
		insert element at end
		@param c [in] append character
	*/
	void push_back(CharT c)
	{
		str_.push_back(c);
	}

	/**
		return pointer to null-terminated nonmutable array
	*/
	const CharT *c_str() const { return str_.c_str(); }

	/**
		return pointer to nonmutable array
	*/
	const CharT *data() const { return str_.data(); }

	/**
		return length of sequence
	*/
	size_type length() const CYBOZU_NOEXCEPT { return str_.length(); }

	/**
		return length of sequence
	*/
	size_type size() const CYBOZU_NOEXCEPT { return str_.size(); }

	/**
		return maximum possible length of sequence
	*/
	size_type max_size() const CYBOZU_NOEXCEPT { return str_.max_size(); }

	/**
		determine new length, padding with null elements as needed
	*/
	void resize(size_type newSize) { str_.resize(newSize); }

	/**
		determine new length, padding with c elements as needed
		@param newSize [in] new length
		@param c [in] initial character
	*/
	void resize(size_type newSize, CharT c)
	{
		str_.resize(newSize, c);
	}

	/**
		return current length of allocated storage
	*/
	size_type capacity() const CYBOZU_NOEXCEPT { return str_.capacity(); }

	/**
		determine new minimum length of allocated storage
		@param newSize [in] reserve size
	*/
	void reserve(size_type newSize = 0) { str_.reserve(newSize); }

	/**
		test if sequence is empty
		@return true if empty
	*/
	bool empty() const CYBOZU_NOEXCEPT { return str_.empty(); }

	/**
		copy [off, off + count) to [dest, dest + count)
		@param dest [in] destination
		@param count [in] count of copy
		@param off [in] copy from here
	*/
	size_type copy(CharT *dest, size_type count, size_type off = 0) const
	{
#ifdef _MSC_VER
		return str_._Copy_s(dest, count, count, off);
#else
		return str_.copy(dest, count, off);
#endif
	}

	/**
		exchange contents with rhs
		@param rhs [in] swap string
	*/
	void swap(StringT& rhs) { str_.swap(rhs.str_); }

	/**
		look for rhs beginnng at or after off
		@param rhs [in] target
		@param off [in] search from here
		@return position
	*/
	size_type find(const StringT& rhs, size_type off = 0) const
	{
		return str_.find(rhs.str_, off);
	}

	/**
		look for [str, str + count) beginnng at or after off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of str
	*/
	size_type find(const CharT *str, size_type off, size_type count) const
	{
		return str_.find(str, off, count);
	}

	/**
		look for [str, NUL) beginnng at or after off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type find(const CharT *str, size_type off = 0) const
	{
		return str_.find(str, off);
	}

	/**
		look for c at or after off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type find(CharT c, size_type off = 0) const
	{
		return str_.find(c, off);
	}

	/**
		look for rhs beginning before off
		@param rhs [in] target
		@param off [in] search from here
	*/
	size_type rfind(const StringT& rhs, size_type off = npos) const
	{
		return str_.rfind(rhs.str_, off);
	}

	/**
		look for [str, str + count) beginning before off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of character
	*/
	size_type rfind(const CharT *str, size_type off, size_type count) const
	{
		return str_.rfind(str, off, count);
	}

	/**
		look for [str, NUL) beginning before off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type rfind(const CharT *str, size_type off = npos) const
	{
		return str_.rfind(str, off);
	}

	/**
		look for c before off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type rfind(CharT c, size_type off = npos) const
	{
		return str_.rfind(c, off);
	}

	/**
		look for one of rhs at or after off
		@param rhs [in] target
		@param off [in] search from here
	*/
	size_type find_first_of(const StringT& rhs, size_type off = 0) const
	{
		return str_.find_first_of(rhs.str_, off);
	}

	/**
		look for one of [str, str + count) at or after off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of character
	*/
	size_type find_first_of(const CharT *str, size_type off, size_type count) const
	{
		return str_.find_first_of(str, off, count);
	}

	/**
		look for one of [str, NUL) at or after off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type find_first_of(const CharT *str, size_type off = 0) const
	{
		return str_.find_first_of(str, off);
	}

	/**
		look for c at or after off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type find_first_of(CharT c, size_type off = 0) const
	{
		return str_.find_first_of(c, off);
	}

	/**
		look for one of rhs before off
		@param rhs [in] target
		@param off [in] search from here
	*/
	size_type find_last_of(const StringT& rhs, size_type off = npos) const
	{
		return str_.find_last_of(rhs.str_, off);
	}

	/**
		look for one of [str, str + count) before off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of character
	*/
	size_type find_last_of(const CharT *str, size_type off, size_type count) const
	{
		return str_.find_last_of(str, off, count);
	}

	/**
		look for one of [str, NUL) before off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type find_last_of(const CharT *str, size_type off = npos) const
	{
		return str_.find_last_of(str, off);
	}

	/**
		look for c before off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type find_last_of(CharT c, size_type off = npos) const
	{
		return str_.find_last_of(c, off);
	}

	/**
		look for none of rhs at or after off
		@param rhs [in] target
		@param off [in] search from here
	*/
	size_type find_first_not_of(const StringT& rhs, size_type off = 0) const
	{
		return str_.find_first_not_of(rhs.str_, off);
	}

	/**
		look for none of [str, str + count) at or after off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of character
	*/
	size_type find_first_not_of(const CharT *str, size_type off, size_type count) const
	{
		return str_.find_first_not_of(str, off, count);
	}

	/**
		look for one of [str, NUL) at or after off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type find_first_not_of(const CharT *str, size_type off = 0) const
	{
		return str_.find_first_not_of(str, off);
	}

	/**
		look for non c at or after off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type find_first_not_of(CharT c, size_type off = 0) const
	{
		return str_.find_first_not_of(c, off);
	}

	/**
		look for none of rhs before off
		@param rhs [in] target
		@param off [in] search from here
	*/
	size_type find_last_not_of(const StringT& rhs, size_type off = npos) const
	{
		return str_.find_last_not_of(rhs.str_, off);
	}

	/**
		look for none of [str, str + count) before off
		@param str [in] target
		@param off [in] search from here
		@param count [in] count of character
	*/
	size_type find_last_not_of(const CharT *str, size_type off, size_type count) const
	{
		return str_.find_last_not_of(str, off, count);
	}

	/**
		look for none of [str, NUL) before off
		@param str [in] target
		@param off [in] search from here
	*/
	size_type find_last_not_of(const CharT *str, size_type off = npos) const
	{
		return str_.find_last_not_of(str, off);
	}

	/**
		look for non c before off
		@param c [in] target
		@param off [in] search from here
	*/
	size_type find_last_not_of(CharT c, size_type off = npos) const
	{
		return str_.find_last_not_of(c, off);
	}
	/**
		return [off, off + count) as new string
		@param off [in] from here
		@param count [in] count of substring
	*/
	StringT substr(size_type off = 0, size_type count = npos) const
	{
		return str_.substr(off, count);
	}
	/**
		compare *this with rhs
		@param rhs [in] target
	*/
	int compare(const StringT& rhs) const
	{
		return str_.compare(rhs.str_);
	}

	/**
		compare [off, off + n) with rhs
		@param off [in] from here
		@param n [in] count of lhs
		@param rhs [in] target
	*/
	int compare(size_type off, size_type n, const StringT& rhs) const
	{
		return str_.compare(off, n, rhs.str_);
	}

	/**
		compare [off, off + n) with rhs [rhsOff, rhsOff + count)
		@param off [in] from here
		@param n [in] count of lhs
		@param rhs [in] target
		@param rhsOff [in] target from here
		@param count [in] count of rhs
	*/
	int compare(size_type off, size_type n, const StringT& rhs, size_type rhsOff, size_type count) const
	{
		return str_.compare(off, n, rhs.str_, rhsOff, count);
	}

	/**
		compare [0, _Mysize) with [str, NUL)
		@param str [in] target
	*/
	int compare(const CharT *str) const
	{
		return str_.compare(str);
	}
	int compare(const Char16 *str) const
	{
		const StringT rhs(str);
		return str_.compare(rhs.str_);
	}

	/**
		compare [off, off + n) with [str, NUL)
		@param off [in] from here
		@param n [in] count of lhs
		@param str [in] target
	*/
	int compare(size_type off, size_type n, const CharT *str) const
	{
		return str_.compare(off, n, str);
	}

	/**
		compare [off, off + n) with [str, str + count)
		@param off [in] from here
		@param n [in] count of lhs
		@param str [in] target
		@param count [in] count of rhs
	*/
	int compare(size_type off,size_type n, const CharT *str, size_type count) const
	{
		return str_.compare(off, n, str, count);
	}
	/**
		convert to std::string with UTF-8
	*/
	void toUtf8(std::string& str) const
	{
		for (size_t i = 0, n = str_.size(); i < n; i++) {
			if (!cybozu::string::AppendUtf8(str, str_[i])) {
				throw cybozu::Exception("string:toUtf8") << i;
			}
		}
	}
	std::string toUtf8() const
	{
		std::string str;
		toUtf8(str);
		return str;
	}
	/**
		convert to std::string with UTF-16LE
	*/
	void toUtf16(cybozu::String16& str) const
	{
		for (size_t i = 0, n = str_.size(); i < n; i++) {
			if (!cybozu::string::AppendUtf16(str, str_[i])) {
				throw cybozu::Exception("string:toUtf16") << i;
			}
		}
	}
	cybozu::String16 toUtf16() const
	{
		cybozu::String16 str;
		toUtf16(str);
		return str;
	}
	/**
		is this valid unicode string?
		@return true correct string
		@return false bad string
	*/
	bool isValid() const
	{
		for (size_t i = 0, n = str_.size(); i < n; i++) {
			if (!cybozu::string::IsValidChar(str_[i])) return false;
		}
		return true;
	}
#if CYBOZU_CPP_VERSION >= CYBOZU_CPP_VERSION_CPP11
	StringT(StringT&& rhs) CYBOZU_NOEXCEPT
	{
		assign(std::forward<StringT>(rhs));
	}
	StringT& operator=(StringT&& rhs) CYBOZU_NOEXCEPT
	{
		assign(std::forward<StringT>(rhs));
		return *this;
	}
	StringT& assign(StringT&& rhs) CYBOZU_NOEXCEPT
	{
		str_.assign(std::forward<BasicString>(rhs.str_));
		return *this;
	}
	const_iterator cbegin() const CYBOZU_NOEXCEPT { return begin(); }
	const_iterator cend() const CYBOZU_NOEXCEPT { return end(); }
	const_reverse_iterator crbegin() const CYBOZU_NOEXCEPT { return rbegin(); }
	const_reverse_iterator crend() const CYBOZU_NOEXCEPT { return rend(); }
	void shrink_to_fit() { str_.shrink_to_fit(); }
	void pop_back() { str_.erase(str_.size() - 1, 1); }
	reference front() { return operator[](0); }
	const_reference front() const { return str_.front(); }
	reference back() { return str_.back(); }
	const_reference back() const { return str_.back(); }
#endif
	/**
		get internal const str(don't use this function)
	*/
	const BasicString& get() const { return str_; }
	/**
		get internal str(don't use this function)
	*/
	BasicString& get() { return str_; }
#if CYBOZU_CPLUSPLUS >= 202002L
	template<class T>bool operator==(const T& rhs) const { return compare(rhs) == 0; }
	template<class T>std::strong_ordering operator<=>(const T& rhs) const {
		int r = compare(rhs);
		return r < 0 ? std::strong_ordering::less : r > 0 ? std::strong_ordering::greater : std::strong_ordering::equal;
	}
#else
	template<class T>bool operator==(const T& rhs) const { return compare(rhs) == 0; }
	template<class T>bool operator!=(const T& rhs) const { return compare(rhs) != 0; }
	template<class T>bool operator<=(const T& rhs) const { return compare(rhs) <= 0; }
	template<class T>bool operator>=(const T& rhs) const { return compare(rhs) >= 0; }
	template<class T>bool operator<(const T& rhs) const { return compare(rhs) < 0; }
	template<class T>bool operator>(const T& rhs) const { return compare(rhs) > 0; }
#endif
private:
	template<class Iterator>
	cybozu::Char getOneChar(Iterator& begin, const Iterator& end)
	{
		return getOneCharSub(begin, end, *begin);
	}
	// dispatch
	template<class Iterator>
	cybozu::Char getOneCharSub(Iterator& begin, const Iterator&, cybozu::Char)
	{
		return *begin++;
	}
	template<class Iterator>
	cybozu::Char getOneCharSub(Iterator& begin, const Iterator& end, char)
	{
		cybozu::Char c;
		if (!cybozu::string::GetCharFromUtf8(&c, begin, end)) {
			throw cybozu::Exception("string:getOneCharSub");
		}
		return c;
	}
	BasicString str_;
};

typedef StringT<cybozu::Char> String;

inline std::istream& getline(std::istream& is, String& str, const char delim = '\n')
{
	std::string tmp;
	std::getline(is, tmp, delim);
	str.assign(tmp);
	return is;
}

inline std::istream& operator>>(std::istream& is, String& str)
{
	std::string tmp;
	is >> tmp;
	str.assign(tmp);
	return is;
}

inline std::ostream& operator<<(std::ostream& os, const String& str)
{
	return os << str.toUtf8();
}

#if CYBOZU_CPLUSPLUS < 202002L
inline bool operator==(const cybozu::Char* lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const cybozu::Char* lhs, const String& rhs) { return rhs != lhs; }
inline bool operator<=(const cybozu::Char* lhs, const String& rhs) { return rhs >= lhs; }
inline bool operator>=(const cybozu::Char* lhs, const String& rhs) { return rhs <= lhs; }
inline bool operator<(const cybozu::Char* lhs, const String& rhs) { return rhs > lhs; }
inline bool operator>(const cybozu::Char* lhs, const String& rhs) { return rhs < lhs; }

inline bool operator==(const cybozu::Char16* lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const cybozu::Char16* lhs, const String& rhs) { return rhs != lhs; }
inline bool operator<=(const cybozu::Char16* lhs, const String& rhs) { return rhs >= lhs; }
inline bool operator>=(const cybozu::Char16* lhs, const String& rhs) { return rhs <= lhs; }
inline bool operator<(const cybozu::Char16* lhs, const String& rhs) { return rhs > lhs; }
inline bool operator>(const cybozu::Char16* lhs, const String& rhs) { return rhs < lhs; }

inline bool operator==(const std::string& lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const std::string& lhs, const String& rhs) { return rhs != lhs; }
inline bool operator<=(const std::string& lhs, const String& rhs) { return rhs >= lhs; }
inline bool operator>=(const std::string& lhs, const String& rhs) { return rhs <= lhs; }
inline bool operator<(const std::string& lhs, const String& rhs) { return rhs > lhs; }
inline bool operator>(const std::string& lhs, const String& rhs) { return rhs < lhs; }

inline bool operator==(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs != lhs; }
inline bool operator<=(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs >= lhs; }
inline bool operator>=(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs <= lhs; }
inline bool operator<(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs > lhs; }
inline bool operator>(const cybozu::String::BasicString& lhs, const String& rhs) { return rhs < lhs; }
#ifdef _MSC_VER
inline bool operator==(const std::wstring& lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const std::wstring& lhs, const String& rhs) { return rhs != lhs; }
inline bool operator<=(const std::wstring& lhs, const String& rhs) { return rhs >= lhs; }
inline bool operator>=(const std::wstring& lhs, const String& rhs) { return rhs <= lhs; }
inline bool operator<(const std::wstring& lhs, const String& rhs) { return rhs > lhs; }
inline bool operator>(const std::wstring& lhs, const String& rhs) { return rhs < lhs; }
#endif
#endif

inline void swap(String& lhs, String& rhs) { lhs.swap(rhs); }

inline String operator+(const String& lhs, const String& rhs) { return String(lhs) += rhs; }
#if CYBOZU_CPP_VERSION >= CYBOZU_CPP_VERSION_CPP11
inline String operator+(String&& lhs, const String& rhs) { return std::move(lhs.append(rhs)); }
#endif

template<class Iter16>
bool ConvertUtf16ToUtf8(std::string *out, Iter16 begin, Iter16 end)
{
	out->clear();
	out->reserve((end - begin) * 3);
	while (begin != end) {
		cybozu::Char c;
		if (!string::GetCharFromUtf16(c, begin, end)) return false;
		if (!string::AppendUtf8(*out, c)) return false;
	}
	return true;
}
inline bool ConvertUtf16ToUtf8(std::string *out, const cybozu::String16& in)
{
	return ConvertUtf16ToUtf8(out, &in[0], &in[0] + in.size());
}

template<class Iter8>
bool ConvertUtf8ToUtf16(cybozu::String16 *out, Iter8 begin, Iter8 end)
{
	out->clear();
	out->reserve((end - begin) / 2);
	while (begin != end) {
		cybozu::Char c;
		if (!string::GetCharFromUtf8(&c, begin, end)) return false;
		if (!string::AppendUtf16(*out, c)) return false;
	}
	return true;
}

inline bool ConvertUtf8ToUtf16(cybozu::String16 *out, const std::string& in)
{
	return ConvertUtf8ToUtf16(out, &in[0], &in[0] + in.size());
}

inline cybozu::String16 ToUtf16(const std::string& in)
{
	cybozu::String16 out;
	if (ConvertUtf8ToUtf16(&out, in)) return out;
	throw cybozu::Exception("string:ToUtf16:bad utf8") << in;
}

inline std::string ToUtf8(const cybozu::String16& in)
{
	std::string out;
	if (ConvertUtf16ToUtf8(&out, in)) return out;
	throw cybozu::Exception("string:ToUtf8:bad utf16");
}

template<class Iterator>
class Utf8refT {
	Iterator begin_;
	Iterator end_;
	bool ignoreBadChar_;
public:
	Utf8refT(Iterator begin, Iterator end, bool ignoreBadChar = false)
		: begin_(begin)
		, end_(end)
		, ignoreBadChar_(ignoreBadChar)
	{
	}
	/*
		get character and seek next pointer
	*/
	bool next(Char *c)
	{
	RETRY:
		if (begin_ == end_) return false;
		bool b = string::GetCharFromUtf8(c, begin_, end_);
		if (b) return true;
		if (ignoreBadChar_) goto RETRY;
		throw cybozu::Exception("string:Utf8ref:getAndNext");
	}
};

struct Utf8ref : Utf8refT<const char*> {
	Utf8ref(const char *begin, const char *end, bool ignoreBadChar = false) : Utf8refT<const char*>(begin, end, ignoreBadChar) {}
	Utf8ref(const char *str, size_t size, bool ignoreBadChar = false) : Utf8refT<const char*>(str, str + size, ignoreBadChar) {}
	explicit Utf8ref(const std::string& str, bool ignoreBadChar = false) : Utf8refT<const char*>(str.c_str(), str.c_str() + str.size(), ignoreBadChar) {}
};

template<class InputStream>
void load(cybozu::String& str, InputStream& is)
{
	loadPodVec(str, is);
}

template<class OutputStream>
void save(OutputStream& os, const cybozu::String& str)
{
	savePodVec(os, str);
}

} // cybozu

// specialization for boost::hash
namespace boost {

template<>
struct hash<cybozu::String> {
	typedef cybozu::String argument_type;
	typedef size_t result_type;
	size_t operator()(const cybozu::String& str) const
	{
		return static_cast<size_t>(cybozu::hash64(str.c_str(), str.size()));
	}
};

} // boost

namespace std {

#if defined(_MSC_VER) && (_MSC_VER == 1600)
	// defined in std ?
#else
CYBOZU_NAMESPACE_TR1_BEGIN
#endif

template<>
struct hash<cybozu::String> {
	typedef cybozu::String argument_type;
	typedef size_t result_type;
	size_t operator()(const cybozu::String& str) const
	{
		return static_cast<size_t>(cybozu::hash64(str.c_str(), str.size()));
	}
};

#if defined(_MSC_VER) && (_MSC_VER == 1600)
	// defined in std ?
#else
CYBOZU_NAMESPACE_TR1_END
#endif

} // std

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
