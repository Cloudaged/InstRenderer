
#ifndef INSTRENDERER_PATHDEFINE_H
#define INSTRENDERER_PATHDEFINE_H

#define FILE_PATH(str) (std::string(PROJECT_ROOT) + "\\" + std::string(str))

static std::string GetNameFromPath(std::string path)
{
    size_t dotPos = path.find_last_of('\\');
    if (dotPos == std::string::npos) {
        return "";
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
#endif //INSTRENDERER_PATHDEFINE_H
