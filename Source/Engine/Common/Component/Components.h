
#ifndef INSTRENDERER_COMPONENTS_H
#define INSTRENDERER_COMPONENTS_H

#include "glm/glm.hpp"
#include "string"
#include "../../Render/Material/StandardMaterial.h"
#include "../../Render/Mesh.h"
#include "../Light.h"
struct Transform
{
    glm::vec3 pos = {0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
};

struct Renderable
{
    Mesh* mesh;
    StandardMaterial* material;
};

struct LightComponent
{
    std::string type="Directional";
    glm::vec3 color = {1.0,1.0,1.0};
    float Intensity =1;
    float range = 0;
};

struct CameraComponent
{
    float fov;
};

#endif //INSTRENDERER_COMPONENTS_H
