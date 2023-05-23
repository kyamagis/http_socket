
#include "iostream"
#include "string"

int main()
{
	std::string str;

	str = "Content-length: 100\r\n";
	str += "Content-type: text/plane\r\n";
	str += "\r\n";
	str += "Get Wild";
	std::cout << str;
}