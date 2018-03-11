#include "json.hpp"
#include <iostream>
#include <fstream>


any p_string(any& a)
{
	std::cout << "p_string" << std::endl;
	std::string s = any::as<std::string>(a);
	std::stringstream ss;
	ss << "\"" << s << "\"";
	return any(ss.str());
}

any p_vector(any& a)
{
	std::cout << "p_vector" << std::endl;
	std::vector<any> v = any::as<std::vector<any> >(a);
	std::stringstream ss;
	ss << "[";
	for (std::size_t i = 0; i < v.size(); ++i)
	{
		if (i < v.size() - 1)
		{
			std::cout << p_string(v[i]) << ",";
		}
		else
		{
			std::cout << p_string(v[i]);
		}
	}
	ss << "]";
	return any(ss.str());
}

any p_longint(any& a)
{
	std::cout << "p_longint" << std::endl;
	long int i = any::as<long int>(a);
	std::stringstream ss;
	ss << i;
	return any(ss.str());
}

int main(int argc, char* argv[])
{
	Json j;

	//j.add_conv(typeid(std::string), p_string);
	//j.add_conv(typeid(long int), p_longint);
	//j.add_conv(typeid(std::vector<any>), p_vector);

	//j["k1"] = 43.4f;
	//j["k2"] = true;
	////bool arr[] = { true, false, false, true };
	////j["k3"] = arr;
	//j["lastname"] = "Navaja";
	//j["age"] = 4.0d;
	//j["phone"]["cellphone"] = "123456";
	//j["phone"]["line"] = "456789";

	////std::string arr[] = { "hola", "mundo" };
	////j["list"] = arr;

	//std::cout << j.str() << std::endl;

	std::ifstream infile;
	infile.open (argv[1]);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	std::string s = buffer.str();
	std::cout << s << std::endl;

	j.parse(s);

	std::cout << j << std::endl;

	std::cout << "---------------------------------------" << std::endl;

	any val = (j["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["GlossDef"]["GlossSeeAlso"]).value();
	std::vector<any> v = any::as<std::vector<any> >(val);
	std::vector<any>::const_iterator it = v.begin();

	std::cout << "GlossSeeAlso: " << std::endl;
	for(; it != v.end(); ++it)
	{
		std::string s = any::as<std::string>(*it);
		std::cout << s << std::endl;
	}

	return 0;
}
