
#ifndef INSTRENDERER_COMPONENTS_H
#define INSTRENDERER_COMPONENTS_H

#include "glm/glm.hpp"
#include "string"
#include "../../Render/Material/PBRMaterial.h"
#include "../../Render/Mesh.h"

struct Transform
{
    glm::vec3 pos = {0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
};

struct Renderable
{
    Mesh mesh;
    PBRMaterial material;
    glm::vec3 color;
};

struct CameraComponent
{
    float fov;
};

#endif //INSTRENDERER_COMPONENTS_H
