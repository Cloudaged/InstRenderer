
#ifndef INSTRENDERER_RESBASE_H
#define INSTRENDERER_RESBASE_H
#include "string"
namespace Res
{
    enum class ResType
    {
        Model,Texture,Material,Animation,Skybox,Shader
    };

    class ResBase
    {
    public:
        ResBase(std::string path,ResType type);
        std::string path;
        ResType type;
    };

} // Res

#endif //INSTRENDERER_RESBASE_H
