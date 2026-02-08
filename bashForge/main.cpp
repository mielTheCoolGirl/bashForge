#define _CRT_SECURE_NO_WARNINGS
#define INVALID_CMD_SYNTAX 5
#define NO_DASH_FOUND -1
#define CMD_DOESNT_EXIST 0
#define NO_SUCH_FILE -2
#define UNABLE_TO_GET_FT -3
#define UNABLE_TO_SET_FT -4
#include "FileData.h"
#include "DateParser.h"
#include "CommandCd.h"
#include <fstream>


std::vector<std::string> getCmdSyntax(const std::string& input)
{
	if (input.empty())
		throw CMD_DOESNT_EXIST;

	std::vector<std::string> cmd;
	std::stringstream stringStream(input);
	std::string currArg;
	bool inQuotes = false;

	for (size_t i = 0; i < input.size(); i++)
	{
		char c = input[i];
		if (c == '"')
		{
			inQuotes = !inQuotes;
			continue;
		}

		if (c == ' ' && !inQuotes) //check if its the end of the cmd /arg
		{
			if (!currArg.empty())
			{
				cmd.push_back(currArg);
				currArg.clear();
			}
			continue;

		}
		currArg += c;
	}
	if (!currArg.empty())
		cmd.push_back(currArg);
	if (cmd.empty())
		throw CMD_DOESNT_EXIST;

	std::vector<std::string> finalCmd;
	std::string combinedFlag = "";

	finalCmd.push_back(cmd[0]); // command itself

	bool endFlags = false;
	int i = 1;

	// collect all flags starting with '-'
	while (i < cmd.size())
	{
		if (!cmd[i].empty() && cmd[i][0] == '-')
			combinedFlag += cmd[i].substr(1);
		else
			break;
		i++;

	}
	if (!combinedFlag.empty())
		combinedFlag = "-" + combinedFlag;
	if (combinedFlag != "")
		finalCmd.push_back(combinedFlag);

	// add all remaining arguments
	while (i < cmd.size())
	{
		finalCmd.push_back(cmd[i]);
		i++;
	}

	return finalCmd;
}

void touch(const std::string& flag, const std::string& fileToCreate, const std::string& fileWithNewTS)
{
	bool changeAccessTime = false, changeModTime = false, dontCreateNonExisting = false, useDate = false, useTime = false, useFilesTS = false, affectSymLinks = false;

	LPCSTR lpstr = fileToCreate.c_str();
	for (char part : flag)
	{
		if (part == '-') continue;
		switch (part)
		{
		case 'a': changeAccessTime = true; break;
		case 'm': changeModTime = true; break;
		case 'c': dontCreateNonExisting = true; break;
		case 'd': useDate = true; break;
		case 't': useTime = true; break;
		case 'r': useFilesTS = true; break;
		case 'h': affectSymLinks = true; break;
		case 'f': break; //this flag does nothing!
		default:
		{
			std::cout << "Error in flag naming, please check for typos\n";
			return;
		}
		}
	}
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS; //the bk semantics is for handling win dirs
	if (affectSymLinks)
		dwFlagsAndAttributes |= FILE_FLAG_OPEN_REPARSE_POINT;

	// Check if file exists to respect the -c flag
	bool fileExists = (GetFileAttributesA(fileToCreate.c_str()) != INVALID_FILE_ATTRIBUTES);
	if (!fileExists && dontCreateNonExisting)
		return;

	HANDLE hFile = INVALID_HANDLE_VALUE;//set the default to be invalid
	if (useTime || useDate) //both cant be together
	{
		SYSTEMTIME utcTime;
		FILETIME ft;
		if (useTime)
		{
			SYSTEMTIME newTime;
			try
			{
				newTime = DateParser::parse_touch_t(fileWithNewTS);
			}
			catch (std::runtime_error err)
			{
				std::cout << err.what();
				return;
			}

			if (!TzSpecificLocalTimeToSystemTime(nullptr, &newTime, &utcTime))
			{
				throw UNABLE_TO_GET_FT;
			}

		}
		else
		{
			DateParser date(fileWithNewTS);
			utcTime = date.result_time;
		}

		if (!SystemTimeToFileTime(&utcTime, &ft))
		{
			throw UNABLE_TO_GET_FT;
		}

		bool defaultMode = !changeAccessTime && !changeModTime;
		//just checking if the access or write flags are there
		LPFILETIME pAccess = (changeAccessTime || defaultMode) ? &ft : NULL;
		LPFILETIME pWrite = (changeModTime || defaultMode) ? &ft : NULL;

		HANDLE hTarget = CreateFileA(
			fileToCreate.c_str(),
			FILE_WRITE_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			dwFlagsAndAttributes,
			NULL
		);

		if (hTarget == INVALID_HANDLE_VALUE)
			throw NO_SUCH_FILE;

		if (!SetFileTime(hTarget, NULL, pAccess, pWrite))
		{
			CloseHandle(hTarget);
			throw UNABLE_TO_SET_FT;
		}

		CloseHandle(hTarget);
		return;
	}
	if (useFilesTS)
	{
		LPCSTR srcFile = fileWithNewTS.c_str();
		LPCSTR targetFile = fileToCreate.c_str();
		HANDLE hSource = CreateFileA(srcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL);
		if (hSource == INVALID_HANDLE_VALUE)
			throw NO_SUCH_FILE;

		FILETIME ftCreation, ftAccess, ftWrite;
		if (!GetFileTime(hSource, &ftCreation, &ftAccess, &ftWrite))
		{
			CloseHandle(hSource);
			throw UNABLE_TO_GET_FT;
		}
		CloseHandle(hSource);
		HANDLE hTarget = CreateFileA(targetFile,
			FILE_WRITE_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			dwFlagsAndAttributes,
			NULL);
		if (hTarget == INVALID_HANDLE_VALUE)
			throw NO_SUCH_FILE;
		if (!SetFileTime(hTarget, &ftCreation, &ftAccess, &ftWrite))
		{
			CloseHandle(hTarget);
			throw UNABLE_TO_SET_FT;
		}

		CloseHandle(hTarget);
		return;
	}
	if (changeAccessTime || changeModTime)
	{
		hFile = CreateFileA(lpstr,
			FILE_WRITE_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			dwFlagsAndAttributes,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			throw NO_SUCH_FILE;
		}

		SYSTEMTIME sysTime;
		FILETIME ftNow;
		GetLocalTime(&sysTime);
		SystemTimeToFileTime(&sysTime, &ftNow);

		LPFILETIME pAccess = changeAccessTime ? &ftNow : NULL;
		LPFILETIME pWrite = changeModTime ? &ftNow : NULL;
		if (!SetFileTime(hFile, NULL, pAccess, pWrite))
		{
			CloseHandle(hFile);
			throw UNABLE_TO_SET_FT;
		}

		CloseHandle(hFile);
		return;
	}
	else
	{
		bool fileExists = (GetFileAttributesA(lpstr) != INVALID_FILE_ATTRIBUTES);

		if (!fileExists && dontCreateNonExisting)
			return;

		hFile = CreateFileA(lpstr,
			FILE_WRITE_ATTRIBUTES | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			fileExists ? OPEN_EXISTING : OPEN_ALWAYS,
			dwFlagsAndAttributes,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw NO_SUCH_FILE;

		SYSTEMTIME now;
		FILETIME ft;

		GetLocalTime(&now);
		SYSTEMTIME utc;
		TzSpecificLocalTimeToSystemTime(NULL, &now, &utc);
		SystemTimeToFileTime(&utc, &ft);
		if (!SetFileTime(hFile, NULL, &ft, &ft))
		{
			CloseHandle(hFile);
			throw UNABLE_TO_SET_FT;
		}
		CloseHandle(hFile);
	}
}

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

void echo(const std::string& echoStr, const std::string& flag)
{
	bool interpretEscapes = true;
	bool newLine = true;
	for (char part : flag)
	{
		if (part == '-') continue;
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

std::string totalAmountInPathKB(const std::string& path)
{
	//in the unix filesys the "total" in a sum of all of its blocks, a block is (size+511)/512
	int sumOfBlocks = 0, currSize = 0;
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

void ls(const std::string& flag, std::string path) //path is optional, only for recursion
{
	if (path.length() == 0)
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
		if (part == '-') continue;
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
		std::sort(entries.begin(), entries.end(), compareByTime);

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
				ls("R", entry.path().string());
			}
		}
	}
}

void cat(const std::string& flag, const std::string& file)
{
	//numbering lines includes empty lines
	bool numberLines = false, showNonPrintables = false, numberNonBlanks = false, sqeezeLines = false, showLineEndings = false, showTabsAsI = false, showAllSpecials;
	std::ifstream fileToCat(file);
	if (!fileToCat.is_open())
	{
		throw NO_SUCH_FILE;
	}
	for (char part : flag)
	{
		if (part == '-') continue;
		switch (part)
		{
		case 'E': showLineEndings = true; break;
		case 's': sqeezeLines = true; break;
		case 'b': numberNonBlanks = true; break;
		case 'T': showTabsAsI = true; break;
		case 'A': showTabsAsI = true, showLineEndings = true; break;
		case 'n': numberLines = true; break;
		case 'v': showNonPrintables = true; break;
		default:
		{
			std::cout << "Error in flag naming, please check for typos\n";
			return;
		}
		}
	}
	std::string currLine;
	int counter = 1;
	if (numberLines && numberNonBlanks)
	{
		numberLines = false; //since the -b flag always overrides the -n flag
	}
	bool lastOneBlank = false;
	while (std::getline(fileToCat, currLine))
	{
		if (showNonPrintables)
		{
			std::string resLine = "";
			for (char c : currLine)
			{
				if ((int(c) >= 0 && int(c) <= 31) || int(c) == 127)
				{
					if (c == 127)
						resLine += "^?";
					else
					{
						c = char(int(c + 0x40));
						resLine += std::string("^") + c;
					}

				}
				else
				{
					resLine += c;
				}

			}
			currLine = resLine;
		}

		if (showTabsAsI)
		{
			size_t posOfTab = 0;
			while (posOfTab = currLine.find("\t", posOfTab) != std::string::npos)
			{
				currLine.replace(posOfTab, 1, "^I");
				posOfTab += 2;
			}
		}
		if (showLineEndings)
		{
			currLine += "$";
		}

		if (numberLines)
		{
			if (!(sqeezeLines && lastOneBlank && (currLine == "$" && showLineEndings) || (currLine == "")))
			{

				std::cout << counter << " " << currLine << std::endl;
				counter++;

			}
		}
		else if (numberNonBlanks)
		{

			if (showLineEndings && currLine == "$")
			{
				if (!(sqeezeLines && lastOneBlank))
				{
					std::cout << currLine << std::endl;
				}
				lastOneBlank = true;
			}

			else if (currLine != "")
			{
				std::cout << counter << " " << currLine << std::endl;
				counter++;
			}

		}

		else
		{

			if (!(sqeezeLines && currLine == "" && lastOneBlank))
			{
				std::cout << currLine << std::endl;
			}
			if (currLine == "")
				lastOneBlank = true;
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


void analyse_input(const std::string& input)
{
	try
	{
		std::vector<std::string> cmdRes = getCmdSyntax(input);
		std::string flag;
		if (cmdRes.size() > 1)
		{
			if (cmdRes[1][0] == '-')
				flag = cmdRes[1];
			else
				flag = "";
		}
		else
			flag = "";

		if (cmdRes[0] == "pwd")
		{
			if (cmdRes.size() > 1)
				throw INVALID_CMD_SYNTAX;
			std::cout << pwd() << "\n";
		}

		else if (cmdRes[0] == "ls")
		{
			if (cmdRes.size() > 1 && flag != "" && cmdRes[1][0] != '-')
				throw NO_DASH_FOUND;

			ls(flag, "");

		}

		else if (cmdRes[0] == "whoami")
		{
			if (cmdRes.size() > 1)
				throw INVALID_CMD_SYNTAX;
			whoami();
		}

		else if (cmdRes[0] == "clear")
		{
			if (cmdRes.size() > 1)
				throw INVALID_CMD_SYNTAX;
			std::string moveCursorBack = "\033[H", clearScreen = "\033[2J", clearScrollBack = "\033[3J";
			std::cout << moveCursorBack + clearScreen + clearScrollBack << std::flush;
		}

		else if (cmdRes[0] == "echo")
		{
			std::string inputed;
			size_t startIdx = (flag.empty() ? 1 : 2);
			if (startIdx >= cmdRes.size())
			{
				inputed = "";
			}
			else
			{
				for (size_t i = startIdx; i < cmdRes.size(); ++i)
				{
					inputed += cmdRes[i];
					if (i != cmdRes.size() - 1)
						inputed += " "; // preserve spaces
				}
			}

			echo(inputed, flag);
		}
		else if (cmdRes[0] == "cat")
		{
			if (cmdRes.size() < 2)
				throw INVALID_CMD_SYNTAX;
			if (cmdRes.size() == 2)
			{
				if (flag != "")
					throw INVALID_CMD_SYNTAX;
				cat(flag, cmdRes[1]);
			}
			else if (cmdRes.size() == 3)
			{
				if (flag == "")
					throw NO_DASH_FOUND;
				cat(flag, cmdRes[2]);
			}
			else
				throw INVALID_CMD_SYNTAX;

		}
		else if (cmdRes[0] == "touch")
		{
			switch (cmdRes.size())
			{
			case 2:
			{
				if (flag != "")
					throw INVALID_CMD_SYNTAX;
				touch(flag, cmdRes[1], "");
				break;
			}
			case 3:
			{
				if (flag == "")
					throw NO_DASH_FOUND;
				touch(flag, cmdRes[2], "");
				break;
			}
			case 4:
			{
				if (flag == "")
					throw NO_DASH_FOUND;

				std::string dateString = cmdRes[2];
				std::string fileToModify = cmdRes[3];

				touch(flag, fileToModify, dateString);

				break;
			}
			default:throw INVALID_CMD_SYNTAX;
			}

		}
		else if (cmdRes[0]=="cd")
		{
			std::string dirToReach;
			if (cmdRes.size() > 2)
				throw INVALID_CMD_SYNTAX;
			if (cmdRes.size() == 1)
				dirToReach = "";
			else
				dirToReach = cmdRes[1];
			CommandCd cd;
			cd.cd(dirToReach);
		}
		else if (cmdRes[0] == "exit")
		{
			return;
		}
		else
			throw(CMD_DOESNT_EXIST);


	}
	
	catch (int errCode)
	{
		switch (errCode)
		{
		case NO_DASH_FOUND: std::cout << "No - found for command, retype your command\n"; break;
		case INVALID_CMD_SYNTAX: std::cout << "Invalid command synthax\n"; break;
		case CMD_DOESNT_EXIST: std::cout << "Error, command doesnt exist!\n"; break;
		case NO_SUCH_FILE: std::cout << "No such file or directory\n"; break;
		case UNABLE_TO_GET_FT: std::cout << "Unable to get file time from file, please try again\n"; break;
		case UNABLE_TO_SET_FT: std::cout << "Unable to set file time from file, please try again\n"; break;
		}
	}
}

int main()
{

	std::string input;
	while (input != "exit")
	{
		std::cout << "> ";
		std::getline(std::cin, input);
		analyse_input(input);
	}
	return 0;
}
