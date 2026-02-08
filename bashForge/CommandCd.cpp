#include "CommandCd.h"


void CommandCd::goHome()
{
    const char* userProfile = getenv("USERPROFILE");
    std::filesystem::path userPath = userProfile;
    std::filesystem::current_path(userPath);
}

bool CommandCd::isDirReachable(std::string& path_str)
{
    std::filesystem::path p(path_str);
    return std::filesystem::exists(p) && std::filesystem::is_directory(p);
}


void CommandCd::cd(std::string& directory)
{
    std::vector<std::string> dirPaths;
    if (directory == ".")
        return;
    if (directory == "/")
    {
        std::filesystem::path userPath = "C:\\"; //path to the root directory
        std::filesystem::current_path(userPath);
        return;
    }

    dirPaths = parseDir(directory);
    if (dirPaths[0] == "~" || dirPaths[0] == "") //go to home
    {
        goHome();
    }
    
        
    for (int i=0;i<dirPaths.size();i++)
    {
        if (dirPaths[i] == "~") 
        {
            goHome();
        }
        if (dirPaths[i] == "..")
        {
            std::filesystem::path parent_dir = std::filesystem::current_path().parent_path();
            std::filesystem::current_path(std::filesystem::canonical(parent_dir));
        }
        else
        {
            if (!isDirReachable(dirPaths[i]))
            {
                //throw a "folder doesnt exist" excpetion
                std::cout << "Error: folder doesn't exist"<<std::endl;
                return;
            }
            else
            {
                std::filesystem::path newPath = std::filesystem::current_path() / dirPaths[i];

                if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath)) 
                {
                    std::filesystem::current_path(newPath);
                }
                else {
                    std::cout << "Error: folder '" << dirPaths[i] << "' doesn't exist" << std::endl;
                    return;
                }
            }
        }
    }
    
}

std::vector<std::string> CommandCd::parseDir(std::string& inputPath)
{
    std::vector<std::string> directories;
    if (inputPath == "")
    {
        directories.push_back(inputPath);
        return directories;
    }
        
    if (inputPath[0] == '~')//if its beginning from home
    {
        directories.push_back(std::to_string(inputPath[0]));
        inputPath = inputPath.substr(1);
    }
    std::string currDir = "";
    for (int i = 0; i < inputPath.size(); i++)
    {
        if (inputPath[i] == '/')
        {
            if (!currDir.empty())
            {
                directories.push_back(currDir);
            }
            currDir = "";
        }
        else
            currDir += inputPath[i];   
    }
    if (!currDir.empty())
        directories.push_back(currDir);
    
    return directories;
}
