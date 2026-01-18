#include "CommandCd.h"



void CommandCd::goHome()
{
    const char* userProfile = getenv("USERPROFILE");
    std::filesystem::path userPath = userProfile;
    std::filesystem::current_path(userPath);
}

bool CommandCd::isDirReachable(std::string& path_str)
{
    const size_t size = 1024;
    char buff[size];
    _getcwd(buff, size);
    std::string path = buff + path_str;
    std::filesystem::path p(path);
    if (std::filesystem::exists(p))
        return true;
    return false;
}


void CommandCd::cd(std::string& directory)
{
    std::vector<std::string> dirPaths;
    if (directory == ".")
        return;

    if (directory == "~" || directory == "") //go to home
    {
        goHome();
    }
    else
    {
        dirPaths = parseDir(directory);
        for (std::string path : dirPaths)
        {

        }
    }
}

std::vector<std::string> CommandCd::parseDir(std::string& inputPath)
{
    std::vector<std::string> directories;
    if (inputPath[0] == '~')//if its beginning from home
    {
        directories.push_back(std::to_string(inputPath[0]));
        inputPath = inputPath.substr(1);
    }
    std::string currDir = "";
    for (int i = 0; i < inputPath.size(); i++)
    {
        while (inputPath[i] != '\\')
        {
            currDir += inputPath[i];
            i++;
        }
        directories.push_back(currDir);
        currDir = "";
    }
    return directories;
}
