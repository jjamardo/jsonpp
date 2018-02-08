#include "json.hpp"
#include <iostream>
#include <fstream>


std::string p_string(any& a)
{
	std::cout << "p_string" << std::endl;
	std::string s = any::as<std::string>(a);
	std::stringstream ss;
	ss << "\"" << s << "\"";
	return ss.str();
}

std::string p_longint(any& a)
{
	std::cout << "p_longint" << std::endl;
	long int i = any::as<long int>(a);
	std::stringstream ss;
	ss << i;
	return ss.str();
}

int main(int argc, char* argv[])
{
	//Json j;

	//j.add_converter(typeid(std::string), p_string);
	//j.add_converter(typeid(long int), p_longint);

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

	/*
	std::string s =
	"{\
		\"lastname\":\"Navaja\",\
		\"name\":\"Pedro\",\
		\"phone\":{\
			\"cellphone\":\"123456\",\
			\"tete\":{,\
				\"totok\":\"totov\",\
			}\
			\"line\":\"456789\"\
		}\
	}";
	*/

	Json j = Json::parse(s);

	std::cout << j << std::endl;

	return 0;
}
