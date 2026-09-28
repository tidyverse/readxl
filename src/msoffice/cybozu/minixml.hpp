#pragma once
/**
	@file
	@brief mini XML parser(not strict)

	@author MITSUNARI Shigeo(@herumi)
*/

#include <assert.h>
#include <cybozu/exception.hpp>
#include <map>
#include <istream>
#include <list>

namespace cybozu {

namespace minixml {

const int indentWidth = 3;

inline std::string escape(const std::string& str)
{
	std::string ret;
	for (size_t i = 0, n = str.size(); i < n; i++) {
		char c = str[i];
		switch (c) {
		case '&':
			ret += "&amp;";
			break;
		case '<':
			ret += "&lt;";
			break;
		case '>':
			ret += "&gt;";
			break;
		case '\'':
			ret += "&apos;";
			break;
		case '"':
			ret += "&quot;";
			break;
		default:
			ret += c;
			break;
		}
	}
	return ret;
}

inline std::string unescape(const std::string& str)
{
	std::string ret;
	for (size_t i = 0, n = str.size(); i < n; i++) {
		char c = str[i];
		if (c == '&') {
			if (n - i >= 5 && memcmp(&str[i + 1], "amp;", 4) == 0) {
				ret += '&';
				i += 4;
			} else if (n - i >= 4 && memcmp(&str[i + 1], "lt;", 3) == 0) {
				ret += '<';
				i += 3;
			} else if (n - i >= 4 && memcmp(&str[i + 1], "gt;", 3) == 0) {
				ret += '>';
				i += 3;
			} else if (n - i >= 6 && memcmp(&str[i + 1], "apos;", 5) == 0) {
				ret += '\'';
				i += 5;
			} else if (n - i >= 6 && memcmp(&str[i + 1], "quot;", 5) == 0) {
				ret += '"';
				i += 5;
			} else {
				throw cybozu::Exception("xml:unescape") << str.substr(i, 8);
			}
		} else {
			ret += c;
		}
	}
	return ret;
}

class Attributes {
	typedef std::map<std::string, std::string> Map;
	Map m_;
public:
	const std::string *query(const std::string& key) const
	{
		Map::const_iterator i = m_.find(key);
		if (i != m_.end()) return &(i->second);
		return 0;
	}
	const std::string& operator[](const std::string& key) const
	{
		const std::string *p = query(key);
		if (p) return *p;
		throw cybozu::Exception("xml:Attributes:no key") << key;
	}
	std::string& operator[](const std::string& key)
	{
		return m_[key];
	}
	void put(int level = 0) const
	{
		std::string tabstr(level + indentWidth, ' ');
		const char *tab = tabstr.c_str();
		for (Map::const_iterator i = m_.begin(), ie = m_.end(); i != ie; ++i) {
			printf("%s%s=%s\n", tab, i->first.c_str(), i->second.c_str());
		}
	}
	void swap(Attributes& attr) CYBOZU_NOEXCEPT
	{
		m_.swap(attr.m_);
	}
	friend inline std::ostream& operator<<(std::ostream& os, const Attributes& attr)
	{
		for (Attributes::Map::const_iterator i = attr.m_.begin(), ie = attr.m_.end(); i != ie; ++i) {
			os << " " << i->first << "=\"" << i->second << "\"";
		}
		return os;
	}
};

struct Node;
typedef std::list<Node*> NodeList;

struct Node {
	std::string name;
	Attributes attr;
	std::string content;
	Node *parent;
	NodeList child;
	Node()
		: parent(0)
	{
	}
	~Node()
	{
		for (NodeList::iterator i = child.begin(); i != child.end(); ++i) {
			delete *i;
		}
	}
	/*
		swap name and attr
	*/
	Node *appendChild(std::string& name, Attributes& attr)
	{
		child.push_back(0);
		Node *p = new Node();
		child.back() = p;
		p->parent = this;
		p->name.swap(name);
		p->attr.swap(attr);
		return p;
	}
	const Node *getFirstTagByName(const std::string& v) const
	{
		if (name == v) {
			return this;
		}
		for (NodeList::const_iterator i = child.begin(), ie = child.end(); i != ie; ++i) {
			const Node *p = (*i)->getFirstTagByName(v);
			if (p) return p;
		}
		return 0;
	}
	void put(int level = 0) const
	{
		std::string tabstr(level, ' ');
		const char *tab = tabstr.c_str();
		printf("%s<%s>\n", tab, name.c_str());
		if (!content.empty()) {
			printf("%s[content]\n", tab);
			printf("%s%s\n", tab, content.c_str());
		}
		attr.put(level);
		for (NodeList::const_iterator i = child.begin(), ie = child.end(); i != ie; ++i) {
			(*i)->put(level + indentWidth);
		}
	}
	friend inline std::ostream& operator<<(std::ostream& os, const Node& node)
	{
		os << "<" << node.name << node.attr << ">";
		for (NodeList::const_iterator i = node.child.begin(), ie = node.child.end(); i != ie; ++i) {
			os << **i;
		}
		os << "</" << node.name << ">";
		return os;
	}
};

inline bool isSpace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline bool isQuote(char c)
{
	return c == '"' || c == '\'';
}

inline bool isEqual(char c)
{
	return c == '=';
}

inline bool isName(char c)
{
	return !isSpace(c) && !isEqual(c) && !isQuote(c);
}

inline bool isName2(char c)
{
	return !isSpace(c) && !isEqual(c) && !isQuote(c) && c != '>';
}

struct isAttrValue {
	char q_; /* ' or " */
	explicit isAttrValue(char q) : q_(q) { }
	bool operator()(char c) const { return c != q_; }
};

struct isContent {
	bool operator()(char c) const { return c != '<'; }
};

enum TagType {
	BeginTag, // <...>
	EndTag, // </...>
	CompleteTag // <.../>
};

template<class Iter>
struct InputStream {
	Iter begin_;
	Iter end_;
	InputStream(Iter begin, Iter end)
		: begin_(begin)
		, end_(end)
	{
	}
	bool empty() const { return begin_ == end_; }
	/*
		@note assume not empty
	*/
	char query() const
	{
		assert(!empty());
		return *begin_;
	}
	void inc()
	{
		assert(!empty());
		++begin_;
	}
	char next()
	{
		if (empty()) throw cybozu::Exception("xml:InputStream:empty");
		return *begin_++;
	}
	void skipSpace()
	{
		while (!empty()) {
			if (!isSpace(query())) return;
			inc();
		}
	}
	template<class Cond>
	std::string getWord(Cond f)
	{
		std::string word;
		while (!empty()) {
			char c = query();
			if (!f(c)) break;
			word.push_back(c);
			inc();
		}
		return word;
	}
	/*
		@note assume attributes exists
	*/
	void parseAttribute(std::string *key, std::string *val)
	{
		*key = getWord(isName);
		if (key->empty()) throw cybozu::Exception("xml:parseAttribute:bad key");
		skipSpace();
		char c = next();
		if (isEqual(c)) {
			skipSpace();
			char q = next();
			if (isQuote(q)) {
				*val = getWord(isAttrValue(q));
				c = next();
				if (c == q) return;
			}
		}
		throw cybozu::Exception("xml:parseAttribute") << *key;
	}
	/*
		normal : true is last is '/>'
		         false if last is '?>'
		@retval true if '/>'
		        false if '>'
	*/
	bool parseAttributes(Attributes& attr, bool normal = true)
	{
		for (;;) {
			skipSpace();
			if (empty()) throw cybozu::Exception("xml:parseAttributes:tag is not complete");
			char c = query();
			if (normal) {
				if (c == '>') {
					inc();
					return false;
				}
				if (c == '/') goto LAST;
			} else {
				if (c == '?') goto LAST;
			}
			std::string key, val;
			parseAttribute(&key, &val);
			attr[key] = val;
		}
	LAST:
		inc();
		char c = next();
		if (c != '>') throw cybozu::Exception("xml:parseAttributes:bad tag char") << c;
		return true;
	}
	/*
		pointer is next of '<'
		get tag name
		*isEnd = true if </name>
	*/
	std::string getTagName(bool *isEnd)
	{
		std::string name = getWord(isName);
		if (name.size() <= 1 || name[name.size() - 1] != '>') throw cybozu::Exception("xml:getTagName:invalid tag");
		name.resize(name.size() - 1);
		if (name[0] == '/') {
			*isEnd = true;
			if (name.size() <= 1) throw cybozu::Exception("xml:getTagName:empty name");
			return name.substr(1);
		}
		return name;
	}
	TagType parseTag(std::string& name, Attributes& attr)
	{
		skipSpace();
		char c = next();
		if (c != '<') throw cybozu::Exception("xml:parseTag:bad begin tag") << c;
		if (empty()) throw cybozu::Exception("xml:parseTag:invalid tag1");
		TagType type = BeginTag;
		c = query();
		if (c == '/') {
			type = EndTag;
			inc();
		}
		name = getWord(isName2);
		if (name.empty()) throw cybozu::Exception("xml:parseTag:invalid tag2");
		if (type == EndTag) {
			char c = next();
			if (c != '>') throw cybozu::Exception("xml:parseTag:invalid tag") << name;
			return EndTag;
		}
		bool ret = parseAttributes(attr);
		return ret ? CompleteTag : BeginTag;
	}
	std::string parseContent()
	{
		return getWord(isContent());
	}
	void parseNode(Node *node)
	{
		for (;;) {
			skipSpace();
			if (empty()) throw cybozu::Exception("xml:parseNode:no end tag1") << node->name;
			if (query() != '<') {
				node->content = parseContent();
				if (empty()) throw cybozu::Exception("xml:parseNode:no end tag2") << node->name;
			}
			std::string name;
			Attributes attr;
			TagType type = parseTag(name, attr);
			if (type == EndTag) {
				if (node->name != name) {
					throw cybozu::Exception("xml:parseNode:") << node->name << name;
				}
				return;
			}
			Node *child = node->appendChild(name, attr);
			if (type == BeginTag) {
				parseNode(child);
			}
		}
	}
};

} // minixml

class MiniXml {
	minixml::Attributes xmlAttr_;
	minixml::Node *root_;
	template<class InputStream>
	void readHeader(InputStream& is)
	{
		is.skipSpace();
		std::string w = is.getWord(minixml::isName);
		if (w != "<?xml") throw cybozu::Exception("xml:readHeader:") << w;
		root_ = new minixml::Node();
		is.parseAttributes(xmlAttr_, false);
	}
	MiniXml(const MiniXml&);
	void operator=(const MiniXml&);
public:
	MiniXml()
		: root_(0)
	{
	}
	MiniXml(const char *begin, const char *end)
		: root_(0)
	{
		parse(begin, end);
	}
	template<class InputStream>
	void parse(InputStream& is)
	{
		readHeader(is);
		minixml::TagType type = is.parseTag(root_->name, root_->attr);
		if (type == minixml::EndTag) throw cybozu::Exception("xml:parse:bad end tag") << root_->name;
		if (type == minixml::BeginTag) {
			is.parseNode(root_);
		}
	}
	void parse(const char *begin, const char *end)
	{
		minixml::InputStream<const char*> is(begin, end);
		parse(is);
	}
	const minixml::Node& get() const { return *root_; }
	~MiniXml()
	{
		delete root_;
	}
	friend inline std::ostream& operator<<(std::ostream& os, const MiniXml& xml)
	{
		os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
		os << *xml.root_;
		return os;
	}
};

} // cybozu
