#ifndef _TINYJSON_
#define _TINYJSON_

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include "any.hpp"
#include <vector>
#include "typeconv.hpp"

class bad_json: public std::exception
{
	public:
		bad_json(const std::string & m) : message(m)
		{
		}
		virtual ~bad_json() throw()
		{
		}
		virtual const char* what() const throw()
		{
			return message.c_str();
		}
	protected:
		std::string message;
};

std::string sanitize(const std::string & str)
{
	std::string s(str);
	unsigned int idx = 0;
	int start, end = 0;
	while (idx != s.size())
	{
		switch (s[idx])
		{
		case '\"':
			// scape strings
			idx = s.find('\"', idx + 1) + 1;
			break;
		case ' ':
			// remove white spaces.
			start = idx;
			end = idx;
			while (s[end] == ' ')
			{
				end++;
			}
			s.erase(start, end - start);
			break;
		case '\n':
		case '\t':
		case '\v':
		case '\a':
		case '\b':
		case '\f':
		case '\r':
			// remove special characters
			s.erase(idx, 1);
			break;
		default:
			// next character
			idx++;
			break;
		}
	}
	return s;
}

any str2type(const std::string & str)
{
	std::string s = sanitize(str);
	std::stringstream error;
	error << "error parsing str to type \"" << s << "\" ";
	switch (s[0])
	{
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		char* r;
		// number
		if (s.find('.') != std::string::npos)
		{
			// double
			double n = strtod(s.c_str(), &r);
			if (!*r)
			{
				return any(n);
			}
		}
		else
		{
			// int
			long int n = strtol(s.c_str(), &r, 0);
			if (!*r)
			{
				return any(n);
			}
		}
		error << "bad number";
		break;
	case 't':
	case 'f':
		// bool
		if (s == "true")
			return any(true);
		else if (s == "false")
			return any(false);
		error << "bad boolean";
		break;
	case 'n':
		// null
		if (s == "null")
			return any(0);
		error << "bad null";
		break;
	case '\"':
		// string
		if (s[s.size() - 1] == '\"')
			return any(s.substr(1,s.size()-2));
		error << "bad string";
		break;
	case '[':
		// array
		if (s[s.size() - 1] == ']')
		{
			std::vector<any> v;
			unsigned int idx = 1;
			bool parse_error = false;
			while (idx < s.size() && !parse_error)
			{
				unsigned int comma = s.find(',', idx);
				if (comma == std::string::npos)
				{
					comma = s.size()-1;
				}
				std::string e = s.substr(idx, comma - idx);
				if (e.empty())
				{
					parse_error = true;
					break;
				}
				v.push_back(str2type(e));
				idx = comma + 1;
			}
			if (!parse_error)
			{
				return any(v);
			}
		}
		error << "bad array";
		break;
	default:
		error << "starting char \"" << s[0] << "\"" << " not valid";
		break;
	}
	throw bad_json(error.str());
}

class Json
{
	static type_converter converter;

	public:
		Json& operator[] (std::string& key)
		{
			if (jmap.find(key) != jmap.end())
			{
				Json j;
				jmap.insert(std::pair<std::string, Json>(key, j));
			}
			return jmap[key];
		}

		Json& operator[] (const char* ckey)
		{
			std::string key(ckey);
			if (jmap.find(key) != jmap.end())
			{
				Json j;
				jmap.insert(std::pair<std::string, Json>(key, j));
			}
			return jmap[key];
		}

		Json& operator=(const char* v)
		{
			std::string s(v);
			value = any(s);
			return *this;
		}

		template <typename T>
		Json& operator=(const T& v)
		{
			value = any(v);
			return *this;
		}

		std::string str() const
		{
			std::stringstream ss;
			if (value)
			{
				any a = converter.convert(value);
				return any::as<std::string>(a);
			}
			ss << "{" << std::endl;
			std::map<std::string, Json>::const_iterator it = jmap.begin();
			int count = jmap.size();
			for(; it != jmap.end(); ++it)
			{
				count--;
				ss << "\"" << it->first << "\"" << ":";
				ss << (it->second).str();
				if (count)
				{
					ss << "," << std::endl;
				}
				else
				{
					ss << std::endl;
				}
			}
			ss << "}";
			return ss.str();
		}

		static Json parse(const std::string& str)
		{
			int idx = 0;
			std::string s = sanitize(str);
			std::cout << s << std::endl;
			return Json::parse(s, idx);
		}

		void add_conv(const std::type_info& t, any (* func)(any&))
		{
			std::cout << t.name() << std::endl;
			converter.add_conv(t, func);
		}

	private:

		template <typename T>
		std::string to_str(const T& t)
		{
			any a = converter.convert(t);
			if (a.type_info() != typeid(std::string))
			{
				std::stringstream error;
				error << "converter added for typeid " << typeid(std::string).name();
				error << " is not of the type std::string";
				throw bad_json(error.str());
			}
			return any::as<std::string>(a);
		}

		static Json parse(std::string& s, int& idx)
		{
			std::stringstream error;
			error << "error parsing json \"" << s << "\" ";
			Json j;
			if (s[idx] != '{')
			{
				error << "must start with \"{\"";
				throw bad_json(error.str());
			}
			idx++;

			while (true)
			{
				if (s[idx] == '}')
				{
					return j;
				}

				if (s[idx] == ',')
				{
					idx++;
				}

				// parse key
				std::string key("");
				if (s[idx] != '\"')
				{
					error << "key must start with (\") but char is \"" << s[idx] << "\"";
					throw bad_json(error.str());
				}
				idx++;
				while (s[idx] != '\"')
				{
					key += s[idx];
					idx++;
				}
				idx++;

				std::cout << "key: " << key << std::endl;

				// must be :
				if (s[idx] != ':')
				{
					throw bad_json("error parsing json");
				}
				idx++;

				// parse value
				if (s[idx] == '{')
				{
					// parse object value
					Json jd = parse(s, idx);
					if (s[idx] != '}')
					{
						throw bad_json("error parsing dictionary");
					}
					idx++;
					j[key] = jd;
				}
				else
				{
					// parse type value
					std::string value("");
					if (s[idx] == '\"')
					{
						value += s[idx];
						idx++;
						while (s[idx] != '\"')
						{
							value += s[idx];
							idx++;
						}
					}
					if (s[idx] == '[')
					{
						value += s[idx];
						idx++;
						while (s[idx] != ']')
						{
							value += s[idx];
							idx++;
						}
					}
					while (s[idx] != ',' && s[idx] != '}')
					{
						value += s[idx];
						idx++;
					}
					Json jv;
					jv.value = str2type(value);
					j[key] = jv;
				}
			}
		}

		std::map<std::string, Json> jmap;
		any value;
};

type_converter Json::converter;

std::ostream& operator<<(std::ostream& os, const Json& j)
{
	os << j.str();
	return os;
}

#endif
