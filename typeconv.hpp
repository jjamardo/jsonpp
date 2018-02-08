#ifndef _TYPE_CONVERTER_
#define _TYPE_CONVERTER_

#include "any.hpp"
#include <typeinfo>
#include <string>
#include <map>

#define converters_map std::map<std::string, any (*)(any&)>
#define converters_it converters_map::const_iterator

class type_converter
{
	public:
		template <typename T>
		any convert(const T& t)
		{
			any dst;
			converters_it it = converters.find(typeid(T).name());
			if (it != converters.end())
			{
				any src = t;
				dst = it->second(src);
			}
			return dst;
		}

		void add_conv(const std::type_info& t, any (* func)(any&))
		{
			converters.insert(std::make_pair(t.name(), func));
		}

	private:
		converters_map converters;
};

#endif
