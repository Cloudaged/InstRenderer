
#ifndef INSTRENDERER_PATHDEFINE_H
#define INSTRENDERER_PATHDEFINE_H
#include "iostream"
#include "filesystem"
#define FILE_PATH(str) (std::string(PROJECT_ROOT) + "/" + std::string(str))

static std::string GetNameFromPath(std::string path)
{
    size_t dotPos = path.find_last_of('/');
    if (dotPos == std::string::npos) {
        dotPos = path.find_last_of("\\");
        if(dotPos == std::string::npos)
        {
            std::cout<<"Cant find res name\n";
            return "";
        }
    }
    return path.substr(dotPos + 1);
}

static std::string GetExtension(std::string name)
{
    size_t dotPos = name.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return name.substr(dotPos + 1);
}

static std::string GetPathWithoutName(std::string fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    auto path = fullPath.substr(0, pos)+"/";
    if (pos == std::string::npos) {
        pos = fullPath.find_last_of("\\");
        path = fullPath.substr(0, pos)+"\\";
        if(pos == std::string::npos)
        {
            std::cout<<"Cant find name\n";
            return "";
        }
    }
    return path;
}
#endif //INSTRENDERER_PATHDEFINE_H
