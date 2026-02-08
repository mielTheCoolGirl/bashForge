#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <filesystem>
#include <direct.h>
#include <cstdlib>
#include <iostream>
class CommandCd
{
	public:
		void cd(std::string& directory);
private:
		std::vector<std::string> parseDir(std::string& inputPath);
		void goHome();
		bool isDirReachable(std::string& path_str);
};
