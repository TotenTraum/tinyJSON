#include "JSON.h"

using namespace JSON;

JSONValue& JSONValue::operator=(const JSONValue& val)
{
	var = val.var;
	type = val.type;
	return *this;
}

JSONValue& JSONValue::operator=(const JSONBoolean& val)
{
	type = Types::boolean;
	var = val;
	return *this;
}
JSONValue& JSONValue::operator=(const JSONArray& val)
{
	type = Types::array;
	var = val;
	return *this;
}
JSONValue& JSONValue::operator=(const JSONNumber& val)
{
	type = Types::number;
	var = val;
	return *this;
}
JSONValue& JSONValue::operator=(const JSONObject& val)
{
	type = Types::object;
	var = val;
	return *this;
}
JSONValue& JSONValue::operator=(const JSONString& val)
{
	type = Types::string;
	var = val;
	return *this;
}
template<class T>
JSONValue& JSONValue::operator=(const T& val)
{
	type = Types::undefined;
	return *this;
}

bool JSONValue::isBoolean() const noexcept { return type == Types::boolean; }
bool JSONValue::isNull() const noexcept { return type == Types::null; }
bool JSONValue::isNumber() const noexcept { return type == Types::number; }
bool JSONValue::isString() const noexcept { return type == Types::string; }
bool JSONValue::isObject() const noexcept { return type == Types::object; }
bool JSONValue::isArray() const noexcept { return type == Types::array; }
bool JSONValue::isUndefined() const noexcept { return type == Types::undefined; }

JSONBoolean JSONValue::toBoolean() const noexcept
{
	if (type == Types::boolean)
		return std::get<JSONBoolean>(var);
	else
		return false;
}
JSONNumber JSONValue::toNumber() const noexcept
{
	if (type == Types::number)
		return std::get<JSONNumber>(var);
	else
		return 0;
}
JSONString JSONValue::toString() const noexcept
{
	if (type == Types::string)
		return std::get<JSONString>(var);
	else
		return JSONString();
}
JSONObject JSONValue::toObject() const noexcept
{
	if (type == Types::object)
	{
		return std::get<JSONObject>(var);
	}
	else
		return JSONObject();
}
JSONArray JSONValue::toArray() const noexcept
{
	if (type == Types::array)
		return std::get<JSONArray>(var);
	else
		return JSONArray();
}

JSONObject JSONDocument::Parser::parseObject(char*& ptr)
{
	ptr++;
	JSONObject object;
	skipwhitespaces(ptr);
	while (*ptr != '}')
	{
		skipwhitespaces(ptr);
		object.insert(parsePair(ptr));
		skipwhitespaces(ptr);
		switch (*ptr)
		{
		case ',':
			ptr++;
			continue;
		case '}':
			break;
		default:
			throw std::exception("undefined situation");
		}
	}
	ptr++;
	return object;
}

JSONPair JSON::JSONDocument::Parser::parsePair(char*& ptr)
{
	JSONString key = parseString(ptr);
	skipwhitespaces(ptr);
	if (*ptr == ':')
		ptr++;
	else
		throw std::exception("not founded value");
	skipwhitespaces(ptr);
	JSONValue value = parseValue(ptr);
	return JSONPair(key, value);
}

JSONValue JSONDocument::Parser::parseValue(char*& ptr)
{
	switch (*ptr)
	{
	case '\"':
		return JSONValue(parseString(ptr));
	case '{':
		return JSONValue(parseObject(ptr));
	case '[':
		return JSONValue(parseArray(ptr));
	}
	if (isdigit(*ptr))
		return JSONValue(parseNumber(ptr));
	if (strncmp(ptr, "true", 4) == 0)
	{
		ptr += 4;
		return JSONValue(true);
	}
	if (strncmp(ptr, "false", 5) == 0)
	{
		ptr += 5;
		return JSONValue(false);
	}
	if (strncmp(ptr, "null", 4) == 0)
	{
		ptr += 4;
		return JSONValue();
	}

	throw std::exception("undefined value");
}

JSONArray JSONDocument::Parser::parseArray(char*& ptr)
{
	ptr++;
	JSONArray Array;
	skipwhitespaces(ptr);
	while (*ptr != ']')
	{
		skipwhitespaces(ptr);
		Array.push_back(parseValue(ptr));
		skipwhitespaces(ptr);
		switch (*ptr)
		{
		case ',':
			ptr++;
			continue;
		case ']':
			break;
		default:
			throw std::exception("undefined situation");
		}
	}
	ptr++;
	return Array;
}

JSONNumber JSONDocument::Parser::parseNumber(char*& ptr)
{
	return JSONNumber(strtof(ptr, &ptr));
}

JSONString JSONDocument::Parser::parseString(char*& ptr)
{
	JSONString tmp;
	ptr++;//skip symbol "
	while (*ptr != '\"')
	{
		if (*ptr == '\\')
			tmp += *(ptr++);
		tmp += *(ptr++);
	}
	ptr++;
	return tmp;
}

JSONValue JSONDocument::Parser::parse(const char* buffer)
{
	if (!strlen(buffer)) //if file is empty
		return JSONValue();
	char* ptr = (char*)buffer;
	skipwhitespaces(ptr);
	JSONValue value;
	switch (*ptr)
	{
	case '{':
		return value = parseObject(ptr);
	case '[':
		return value = parseArray(ptr);
	default:
		throw std::exception("this is not JSON");
	}
}

void JSONDocument::Parser::skipwhitespaces(char*& ptr)
{
	while (true)
		switch (*ptr)
		{
		case '\n':
		case '\t':
		case ' ':
			ptr++;
			continue;
		default:
			return;
		}
}

void JSONDocument::readfile(const char* name)
{
	std::ifstream File(name);
	readfile(File);
	File.close();
}

void JSONDocument::readfile(std::istream& File)
{
	File.seekg(0, File.end);
	long long size = File.tellg();
	File.seekg(0, File.beg);
	char* buffer = new char[(int)(++size)];
	File.read(buffer, size);
	delete mainValue;
	mainValue = new JSONValue();
	*mainValue = parser.parse(buffer);
	delete[] buffer;
}

std::ostream& JSONDocument::Writer::setTabs(std::ostream& os)
{
	for (auto i = 0; i < indent; i++)
		os << "\t";
	return os;
}

void JSONDocument::Writer::writeArray(std::ostream& os, const JSON::JSONArray& val)
{
	os << "[";
	indent++;
	if (!compact) os << '\n';
	if (val.size() != 0)
	{
		auto prev = --val.end();
		for (auto iter = val.begin(); iter != prev; iter++)
		{
			if (!compact) setTabs(os);
			writeValue(os, *iter);
			os << ",";
			if (!compact) os << '\n';
		}
		if (!compact) setTabs(os);
		writeValue(os, *prev);
		if (!compact) os << '\n';
	}
	indent--;
	if (!compact) setTabs(os);
	os << "]";
}

void JSONDocument::Writer::writeObject(std::ostream& os, const JSON::JSONObject& val)
{
	os << "{";
	if (!compact) os << '\n';
	indent++;
	if (val.size() != 0)
	{
		auto prev = --val.end();
		for (auto iter = val.begin(); iter != prev; iter++)
		{
			if (!compact) setTabs(os);
			os << "\"" << iter->first << "\" : ";
			writeValue(os, (iter)->second);
			os << ",";
			if (!compact) os << '\n';
		}
		if (!compact) setTabs(os);
		os << "\"" << (prev)->first << "\" : ";
		writeValue(os, (prev)->second);
		if (!compact) os << '\n';
	}
	indent--;
	if (!compact) setTabs(os);
	os << "}";
}

void JSONDocument::Writer::writeValue(std::ostream& os, const JSON::JSONValue& val)
{
	switch (val.getType())
	{
	case JSONValue::Types::boolean:
		os << (val.toBoolean() ? "true" : "false");
		break;
	case JSONValue::Types::null:
		os << "null";
		break;
	case JSONValue::Types::number:
		os << val.toNumber();
		break;
	case JSONValue::Types::string:
		os << "\"" << val.toString() << "\"";
		break;
	case JSONValue::Types::array:
		writeArray(os, val.toArray());
		break;
	case JSONValue::Types::object:
		writeObject(os, val.toObject());
	}
}

bool JSONDocument::writefile(const char* name, bool compact)
{
	std::ofstream File(name);
	writefile(File, compact);
	File.close();
	return true;
}
bool JSONDocument::writefile(std::ostream& File, bool compact)
{
	writer.setStyle(compact);
	writer.resetIndent();
	if (mainValue)
		return false;
	writer.writeValue(File, *mainValue);
	return true;
}