#define _CRT_SECURE_NO_WARNINGS

#include "FileData.h"


//function shows current directory
std::string pwd()
{
	const size_t size = 1024;
	char buff[size];
	if (_getcwd(buff, size) != NULL)
	{
		return buff;
	}
	return "";
}

std::string totalAmountInPathKB(std::string path)
{
	int sumOfBytes = 0;
	for (const auto& entry : std::filesystem::directory_iterator(path))
		sumOfBytes += std::filesystem::file_size(entry.path());
	return std::to_string(sumOfBytes / 1000);
}


void ls(char flag)
{
	std::string path = pwd();
	switch(flag)
	{
	case 'l':
	{
		std::cout << "Total " << totalAmountInPathKB(path)<<"\n";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			FileData currFile(entry.path());
			std::cout<<currFile.collectFilePremissions();
		}	
		break;
	}
	case 'h':
	{

		break;
	}
	default:
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (entry.path().filename().string().front() != '.'&& flag!='a')
				std::cout << entry.path().filename().string() << std::endl;
			else if(flag=='a')
				std::cout << entry.path().filename().string() << std::endl;
	}

	}
}

void whoami()
{
	const char* user = std::getenv("USERNAME");
	if (user)
		std::cout << user << "\n";
	else
		std::cout << "Unknown" << "\n";
}

char parseFlag(std::string input)
{
	return input[4]; //currently hardcoded, fix later to find flag after -
}

void analyse_input(std::string input)
{
	char flag;
	if (input.length() > 3)//TODO: LATER CHECKS WILL NOT CHECK BY LENGTH BUT BY A MAP OF COMMANDS
	{
		flag = parseFlag(input);
	}
	else
		flag = '-';
	
	if (input.find("pwd") != std::string::npos)
		std::cout << pwd() << "\n";
	if (input.find("ls") != std::string::npos)
		ls(flag);
	if (!(input.compare("whoami")))
		whoami();

}

int main()
{

	std::string input;
	while (input != "exit")
	{
		std::cout << "> ";
		std::getline(std::cin,input);
		analyse_input(input);
	}
	return 0;
}