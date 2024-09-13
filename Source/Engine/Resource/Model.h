
#ifndef INSTRENDERER_MODEL_H
#define INSTRENDERER_MODEL_H

#include "vector"
#include "ResMesh.h"

#include "string"
namespace Res
{

    class Model
    {
    public:
        std::vector<ResMesh> meshes;
        std::string path;
    };

} // Res

#endif //INSTRENDERER_MODEL_H
