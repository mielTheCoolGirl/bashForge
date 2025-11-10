#define _CRT_SECURE_NO_WARNINGS

#include "FileData.h"

FileData::FileData(std::filesystem::path path)
{
	this->path = path;
	this->fileSize = std::filesystem::file_size(path);
	this->isdir=std::filesystem::is_directory(path);
	makePremissions();
	findOwnerAndGroup();
	getFullTime();
	countLinks();
}

void FileData::makePremissions()
{
	std::filesystem::perms perm = std::filesystem::status(path).permissions();
	std::string premissions;
	premissions += (this->isdir == 1 ? "d" : "-");
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
	this->permissions = premissions;
}

void FileData::findOwnerAndGroup()
{
	EnvironmentUsers user(path.string());
	this->owner = user.getOwner();
	this->group = user.getGroup();
}

void FileData::getFullTime()
{
	auto time = std::filesystem::last_write_time(path);
	auto timePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(time - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	std::time_t fullTime = std::chrono::system_clock::to_time_t(timePoint);
	std::ostringstream stringTime;
	stringTime << std::put_time(std::localtime(&fullTime), "%b %d %Y %H:%M:%S");
	this->time = stringTime.str();
}

//Getting the number of hard links for the path
void FileData::countLinks()
{
	HANDLE hForFile;
	if (!(isdir))
		hForFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	else
		hForFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
	if (hForFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "SYSTEM ERROR WHILE FETCHING LINKS\n\n";
	}

	BY_HANDLE_FILE_INFORMATION info;
	//fetch number of hard links
	if (GetFileInformationByHandle(hForFile, &info))
	{
		CloseHandle(hForFile);
		this->numOfLinks = info.nNumberOfLinks;
	}

}

std::string FileData::collectFilePremissions()
{
	std::string premissions;
	premissions += this->permissions + " " + std::to_string(numOfLinks) +" " + this->owner + " " + this->group +" " + std::to_string(fileSize) + " " + time + "\t" + path.filename().string() + "\n";
	return premissions;
}
