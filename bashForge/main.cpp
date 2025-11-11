#define _CRT_SECURE_NO_WARNINGS
#define INVALID_CMD_SYNTAX 5
#define NO_DASH_FOUND -1
#define CMD_DOESNT_EXIST 0
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

void echo(std::string echoStr, std::string flag)
{
	bool interpretEscapes = true;
	bool newLine = true;
	for (char part : flag)
	{
		switch (part)
		{
		case 'E': interpretEscapes = false; break;
		case 'e': interpretEscapes = true; break;
		case 'n': newLine = false; break;
		default:
		{
			std::cout << "Error in flag naming, please check for typos\n";
			return;
		}
		}
	}

	if (!interpretEscapes)
	{
		std::cout << echoStr;
		if (newLine) std::cout << "\n";
		return;
	}
	for (size_t i = 0; i < echoStr.size(); ++i)
	{
		if ((i - 1) >= 0)
		{
			if ((echoStr[i] == 't' || echoStr[i] == 'n') && echoStr[i - 1] == '\\')
			{
				switch (echoStr[i]) //-e means that we implement any \n and not treat them as regular strs, we remove the last printed \ char and make a new like\tab
				{
					++i;
				case('n'):std::cout << "\b " << '\n'; break;
				case('t'):std::cout << "\b " << '\t'; break;
				default:std::cout << echoStr[i]; break;
				}

			}

			else
			{
				std::cout << echoStr[i]; 
			}

		}
		
		else
		{
			std::cout << echoStr[i];
		}
		

	}
	if (newLine)
		std::cout << "\n";


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

bool compareByTime(const std::filesystem::directory_entry& first, const std::filesystem::directory_entry& last)
{
	return std::filesystem::last_write_time(first) > std::filesystem::last_write_time(last);
}
void ls(std::string flag,std::string path) //path is optional, only for recursion
{
	if(path.length() == 0)
		path = pwd();
	if (flag.length() == 0)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (entry.path().filename().string().front() != '.')
				std::cout << entry.path().filename().string() << std::endl;
		return;
	}
	bool showHidden = false, longListing = false, humanReadable = false, allFiles = false, recursive = false, reverse = false, timeSort = false;
	for (char part : flag)
	{
		switch (part)
		{
		case 'a':showHidden = true; break;
		case 'l': longListing = true; break;
		case 'h':	humanReadable = true; break;
		case 'R':recursive = true; break;
		case 'r': reverse = true; break;
		case 't':timeSort = true; break;

		default:
		{
			std::cout << "Error in flag naming, please check for typos\n";
			return;
		}
		}
	}
	std::vector<std::filesystem::directory_entry> entries;//get all files in dir
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (showHidden || entry.path().filename().string().front() != '.')
			entries.push_back(entry);
	}
	if (timeSort)
		std::sort(entries.begin(), entries.end(),compareByTime);
	
	if (reverse)
		std::reverse(entries.begin(), entries.end());
	if (longListing)
		std::cout << "Total " << totalAmountInPathKB(path) << "\n";
	for (const auto& entry : entries)
	{
		char flag = '-';
		if (longListing)
			flag = 'l';
		if (humanReadable)
			flag = 'h';

		if (flag == 'l' || flag == 'h')
		{
			FileData currFile(entry.path(), flag);
			std::cout << currFile.collectFilePremissions();
		}
		else
		{
			if (showHidden || entry.path().filename().string().front() != '.')
				std::cout << entry.path().filename().string() << std::endl;

		}
	}
	
	if (recursive)
	{
		//call recursively ls until there are no more directories inside the directories
		for (const auto& entry : entries)
		{
			if (entry.is_directory())
			{
				std::cout << "\n" << entry.path().filename().string() << ":\n";
				ls("R",entry.path().string());
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
	size_t dashIndex = input.find("-");
	if (dashIndex == input.npos)
		return "";
	size_t spaceIndex= input.find(" ",dashIndex);
	if (spaceIndex == input.npos)
		return input.substr(dashIndex + 1);
	return input.substr(dashIndex+1,spaceIndex-dashIndex-1);
}

void analyse_input(std::string input)
{
	std::string flag;
	flag = parseFlag(input);
	try
	{
		if (input.find("pwd") != std::string::npos)
		{
			if (input.length() > 3)
				throw(INVALID_CMD_SYNTAX);
			std::cout << pwd() << "\n";
		}

		else if (input.find("ls") != std::string::npos)
		{
			if (input.length() == 3)
				throw(INVALID_CMD_SYNTAX);
			if (input.length() > 3)
			{
				if (input[3] != '-')//if the is no - placement for command
					throw(NO_DASH_FOUND);
			}
				
			ls(flag,"");

		}

		else if (input.find("whoami") != std::string::npos)
		{
			if (input.length() != 6)
				throw(INVALID_CMD_SYNTAX);
			whoami();
		}

		else if (input.find("clear") != std::string::npos)
		{
			std::string moveCursorBack = "\033[H",clearScreen="\033[2J",clearScrollBack="\033[3J";
			if (input.length() != 5)
				throw(INVALID_CMD_SYNTAX);
			std::cout << moveCursorBack+clearScreen+clearScrollBack<<std::flush;
		}
			
		else if (input.find("echo") != std::string::npos)
		{
			std::string inputed;
			size_t afterSpace= input.find(' '); //find the flag after the space
			
			if (afterSpace == std::string::npos)
				throw(INVALID_CMD_SYNTAX);

			inputed = input.substr(afterSpace+1);
			if (!inputed.empty() && inputed[0] == '-')
			{
				size_t afterSpaceFlagPos = inputed.find(" ");
				if (afterSpaceFlagPos != std::string::npos)
				{
					flag = inputed.substr(1,afterSpaceFlagPos-1);
					inputed = inputed.substr(afterSpaceFlagPos+1);
				}
				else
				{
					flag = inputed.substr(1);
					inputed = "";
				}
			}
			
			echo(inputed,flag);
		}
		else
			throw(CMD_DOESNT_EXIST);

	}
	catch (int errCode)
	{
		switch (errCode)
		{
		case -1: std::cout << "No - found for command, retype your command\n"; break;
		case 5:std::cout << "Invalid command synthax\n"; break;

		}
	}
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