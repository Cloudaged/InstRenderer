
#ifndef INSTRENDERER_RESMODEL_H
#define INSTRENDERER_RESMODEL_H

#include "vector"
#include "ResMesh.h"

#include "string"
namespace Res
{

    class ResModel
    {
    public:
        std::vector<ResMesh> meshes;
        std::string path;
    };

} // Res

#endif //INSTRENDERER_RESMODEL_H
