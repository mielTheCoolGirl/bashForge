#define _CRT_SECURE_NO_WARNINGS


#include <string>
#include <iostream>
#include <filesystem>
#include <direct.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "EnvironmentUsers.h"

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

//Getting the number of hard links per path
int getNumberOfLinks(std::string path,bool isDir)
{
	HANDLE hForFile;
	if(!(isDir))
		hForFile=CreateFileW(std::wstring(path.begin(),path.end()).c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr );
	else
		hForFile = CreateFileW(std::wstring(path.begin(), path.end()).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
	if (hForFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "SYSTEM ERROR WHILE FETCHING LINKS\n\n";
		return -1;
	}

	BY_HANDLE_FILE_INFORMATION info;
	//fetch number of hard links
	if (GetFileInformationByHandle(hForFile, &info))
	{
		CloseHandle(hForFile);
		return info.nNumberOfLinks;
	}
		
	return -1;
}

std::string timeToString(std::filesystem::path path)
{
	auto time = std::filesystem::last_write_time(path);
	auto timePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(time - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	std::time_t fullTime = std::chrono::system_clock::to_time_t(timePoint);
	std::ostringstream stringTime;
	stringTime << std::put_time(std::localtime(&fullTime), "%b %d %Y");
	return stringTime.str();
}
std::string premissionsToString(bool isDir, std::filesystem::path path)
{
	std::filesystem::perms perm = std::filesystem::status(path).permissions();
	std::string premissions;
	premissions += (isDir == 1 ? "d" : "-");
	//check if the premission of the user (owner/group/others) is read/write/exec, if so add wanted flag 
	//---------OWNER PREMISSIONS--------
	if ((perm & std::filesystem::perms::owner_read) != std::filesystem::perms::none)
		premissions += "r";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::owner_write) != std::filesystem::perms::none)
		premissions += "w";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::owner_exec) != std::filesystem::perms::none)
		premissions += "x";
	else
		premissions += "-";
	//---------GROUP PREMISSIONS--------
	if ((perm & std::filesystem::perms::group_read) != std::filesystem::perms::none)
		premissions += "r";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::group_write) != std::filesystem::perms::none)
		premissions += "w";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::group_exec) != std::filesystem::perms::none)
		premissions += "x";
	else
		premissions += "-";

	//---------OTHER PREMISSIONS--------
	if ((perm & std::filesystem::perms::others_read) != std::filesystem::perms::none)
		premissions += "r";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::others_write) != std::filesystem::perms::none)
		premissions += "w";
	else
		premissions += "-";

	if ((perm & std::filesystem::perms::others_exec) != std::filesystem::perms::none)
		premissions += "x";
	else
		premissions += "-";

	premissions += " "+(isDir==1 ? std::to_string(getNumberOfLinks(path.string(),true)) : std::to_string(getNumberOfLinks(path.string(), false)))+" "; //getting number of hard links of file/dir

	EnvironmentUsers user(path.string());
	premissions += user.getOwner() + " " + user.getGroup() + " ";

	premissions += std::to_string(std::filesystem::file_size(path)) + " ";
	return premissions;
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
			std::filesystem::path currPath = entry.path();
			std::cout<<premissionsToString(std::filesystem::is_directory(currPath),currPath) <<timeToString(currPath)<<"\t" << currPath.filename().string() << "\n";
		}	
		break;
	}
	default:
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
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