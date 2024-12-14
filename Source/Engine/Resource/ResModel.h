
#ifndef INSTRENDERER_RESMODEL_H
#define INSTRENDERER_RESMODEL_H

#include "vector"
#include "ResMesh.h"
#include "ResBase.h"
#include "string"
#include <limits>
#include "ResNode.h"
using BoundingPoint = glm::vec3;
namespace Res
{

    class ResModel :public ResBase
    {
    public:
        ResModel(std::string path);
        ResNode* rootNode;
        BoundingPoint minPoint = glm::vec3(std::numeric_limits<float>::max());
        BoundingPoint maxPoint = glm::vec3(std::numeric_limits<float>::lowest());
    private:
    };

} // Res

#endif //INSTRENDERER_RESMODEL_H
