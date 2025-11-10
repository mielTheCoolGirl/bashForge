#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <direct.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "EnvironmentUsers.h"
class FileData
{
private:
	bool isdir;
	int numOfLinks;
	std::filesystem::path path;
	std::string permissions;
	std::string owner, group;
	std::string time;
	size_t fileSize;
public:
	FileData(std::filesystem::path path);
	void makePremissions();
	void findOwnerAndGroup();
	void getFullTime();
	void countLinks();
	std::string collectFilePremissions();
};
