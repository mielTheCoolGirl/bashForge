#include "EnvironmentUsers.h"

EnvironmentUsers::EnvironmentUsers(const std::string& path)
{
	this->path = path;
	findOwnerAndGroup(std::wstring(path.begin(),path.end()));
}

void EnvironmentUsers::findOwnerAndGroup(const std::wstring& path)
{
	PSID ownerSid=nullptr,groupSid=nullptr;
	PSECURITY_DESCRIPTOR pSD=nullptr;
	DWORD result = GetNamedSecurityInfoW(path.c_str(),SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,&ownerSid,&groupSid,nullptr,nullptr,&pSD);
	if (result != ERROR_SUCCESS)
		return;
	WCHAR ownerName[256], ownerDomain[256];
	DWORD ownerNameSize = 256, ownerDomainSize = 256;
	WCHAR groupName[256], groupDomain[256];
	DWORD groupNameSize = 256, groupDomainSize = 256;
	SID_NAME_USE sidType;
	if (LookupAccountSidW(nullptr, ownerSid, ownerName, &ownerNameSize, ownerDomain, &ownerDomainSize, &sidType)) 
	{
		std::wstring wname(ownerName);
		this->owner = std::string(wname.begin(), wname.end());
	}
	//check fi the group sid is null, if its null then the group is "None"
	if (groupSid && LookupAccountSidW(nullptr, groupSid, groupName, &groupNameSize, groupDomain, &groupDomainSize, &sidType))
	{
		std::wstring wname(groupName);
		this->group = std::string(wname.begin(), wname.end());
	}
	else
		this->group = "None";
	if (pSD) LocalFree(pSD);
}

std::string EnvironmentUsers::getOwner() const
{
	return this->owner;
}

std::string EnvironmentUsers::getGroup() const
{
	return this->group;
}
