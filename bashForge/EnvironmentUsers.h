#pragma once
#include <windows.h>
#include <sddl.h>
#include <aclapi.h>
#include <iostream>
#include <string>
class EnvironmentUsers
{
private:
	std::string path;
	std::string owner;
	std::string group;
public:
	EnvironmentUsers(const std::string& path);
	void findOwnerAndGroup(const std::wstring& path);
	std::string getOwner() const;
	std::string getGroup() const;
};
