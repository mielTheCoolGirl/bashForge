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
	//in the unix filesys the "total" in a sum of all of its blocks, a block is (size+511)/512
	int sumOfBlocks = 0,currSize=0;
	int typicalDirBlockSize = 4096;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_regular_file())
			currSize = std::filesystem::file_size(entry.path());
		else if (entry.is_directory())
			currSize = typicalDirBlockSize;
		sumOfBlocks += (currSize + 511) / 512;
	}
		
	return std::to_string(sumOfBlocks);
}


void ls(std::string flag)
{
	std::string path = pwd();
	if (flag.length() == 0)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (entry.path().filename().string().front() != '.')
				std::cout << entry.path().filename().string() << std::endl;
	}
	bool longListing = false, humanReadable = false, allFiles = false , recursive=false,reverse=false,timeSort=false;
	for (char part : flag)
	{
		switch (part)
		{
			case 'l':
			{/*
				std::cout << "Total " << totalAmountInPathKB(path) << "\n";
				for (const auto& entry : std::filesystem::directory_iterator(path))
				{
					FileData currFile(entry.path(), 'l');
					std::cout << currFile.collectFilePremissions();
				}
				*/
				longListing = true;
				break;
			}
			case 'h':
			{
				humanReadable = true;
				break;
			}
			
			case 'R':
			{
				recursive = true;
				break;
			}
			case 'r':
			{
				reverse = true;
				break;
			}
			case 't':
			{
				timeSort = true;
				break;
			}
			default:
			{
				std::cout << "Error in flag naming, please check for typos\n";
				return;
			}
		}
		if (humanReadable && longListing)
		{
			std::cout << "Total " << totalAmountInPathKB(path) << "\n";
				for (const auto& entry : std::filesystem::directory_iterator(path))
				{
					FileData currFile(entry.path(), 'h');
					std::cout << currFile.collectFilePremissions();
				}
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

std::string parseFlag(std::string input)
{
	return input.substr(input.find("-") + 1); ; //currently hardcoded, fix later to find flag after -
}

void analyse_input(std::string input)
{
	std::string flag;
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