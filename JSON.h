#pragma once
#include <map>
#include <list>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <variant>

namespace JSON
{
	class JSONValue;
	using JSONNumber = double;
	using JSONBoolean = bool;
	using JSONString = std::string;
	using JSONPair = std::pair<JSONString, JSONValue>;
	using JSONObject = std::map<JSONString,JSONValue>;
	using JSONArray = std::list<JSONValue>;
	using Variant = std::variant<JSONNumber, JSONBoolean, JSONString,JSONObject,JSONArray>;
	class JSONValue
	{
	public:
		enum class Types
		{
			null = 0,
			number = 1,
			boolean = 2,
			string = 3,
			object = 4,
			array = 5,
			undefined = 80
		};

		JSONValue():var(),type(Types::null) {}
		JSONValue(const JSONValue& tmp)   :type(tmp.type) { var = tmp.var; }
		JSONValue(const JSONArray& val)   :var(val), type(Types::array) {}
		JSONValue(const JSONNumber& val)  :var(val), type(Types::number) {}
		JSONValue(const long long& val)   :var((double)val), type(Types::number) {}
		JSONValue(const int& val)         :var((double)val), type(Types::number) {}
		JSONValue(const JSONString& val)  :var(val), type(Types::string) {}
		JSONValue(const JSONObject& val)  :var(val), type(Types::object) {}
		JSONValue(const JSONBoolean& val) :var(val), type(Types::boolean) {}

		bool isBoolean() const noexcept;
		bool isNull() const noexcept;
		bool isNumber() const noexcept;
		bool isString() const noexcept;
		bool isObject() const noexcept;
		bool isArray() const noexcept;
		bool isUndefined() const noexcept;

		JSONBoolean toBoolean() const noexcept;
		JSONNumber toNumber() const noexcept;
		JSONString toString() const noexcept;
		JSONObject toObject() const noexcept;
		JSONArray toArray() const noexcept;

		JSONValue& operator=(const JSONValue& val);
		JSONValue& operator=(const JSONBoolean& val);
		JSONValue& operator=(const JSONArray& val);
		JSONValue& operator=(const JSONNumber& val);
		JSONValue& operator=(const JSONObject& val);
		JSONValue& operator=(const JSONString& val);
		template<class T>
		JSONValue& operator=(const T& val);
		Types getType() const noexcept {return type;}

	protected:
		Variant var;
		Types type;
	};
	// I/O JSONValue 
	class JSONDocument 
	{
		class Parser 
		{
			JSONObject parseObject(char*&);
			JSONPair parsePair(char*&);
			JSONValue parseValue(char*&);
			JSONArray parseArray(char*&);
			JSONNumber parseNumber(char*&);
			JSONString parseString(char*&);
			void skipwhitespaces(char*&);
		public:
			JSONValue parse(const char*);
		};
		class Writer
		{
			long long indent;//n-tabs
			bool compact;    //style

			//set tabs in ostream
			std::ostream& setTabs(std::ostream&);
			//Write JSONArray
			void writeArray(std::ostream&, const JSON::JSONArray& val);
			//Write JSONObject
			void writeObject(std::ostream&, const JSON::JSONObject& val);
		public:
			Writer() :indent{ 0 }, compact{0} {}
			//Write JSONValue
			void writeValue(std::ostream&, const JSON::JSONValue& val);

			void setStyle(bool isCompact) {compact = isCompact;}
			void resetIndent() { indent = 0; }
		};
	public:
		JSONDocument() {}
		//read from file
		void readfile(const char*);
		//read from istream
		void readfile(std::istream&);
		//Write in file
		bool writefile(const char* fileName,bool isCompact);
		//Write in ostream
		bool writefile(std::ostream&,bool isCompact);
		//return JSONObject 
		JSONObject getObject() 
		{
			if (mainValue->getType() == JSONValue::Types::object)
				return mainValue->toObject();
			else
				return JSONObject();
		}
		//return JSONArray
		JSONArray getArray()
		{
			if (mainValue->getType() == JSONValue::Types::array)
				return mainValue->toArray();
			else
				return JSONArray();
		}
		//set JSONArray
		void setArray(const JSONArray& arr)
		{
			delete mainValue;
			mainValue = new JSONValue();
			*mainValue = arr;
		}
		//set JSONObject
		void setObject(const JSONObject& obj) 
		{
			delete mainValue;
			mainValue = new JSONValue();
			*mainValue = obj; 
		}
	protected:
		Parser parser;
		Writer writer;
		JSONValue* mainValue;
	};
}