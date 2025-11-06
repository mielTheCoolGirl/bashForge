#include <stdio.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <direct.h>

//function shows current directory
std::string pwd()
{
	const size_t size = 1024;
	char buff[size];
	if (_getcwd(buff, size) != NULL)
	{
		std::cout << buff << "\n";
		return buff;
	}
}
void ls()
{
	for (const auto& entry : std::filesystem::directory_iterator(pwd()))
		std::cout << entry.path().filename() << std::endl;
}
void analyse_input(std::string input)
{
	if (input.find("pwd") != std::string::npos)
		pwd();
	else
		ls();


}
int main()
{

	std::string input = "";
	while (input != "exit")
	{
		std::cout << "> ";
		std::cin >> input;
		analyse_input(input);
	}
	return 0;
}