
#ifndef INSTRENDERER_TRANSFORMCOMPONENT_H
#define INSTRENDERER_TRANSFORMCOMPONENT_H
#include "glm/glm.hpp"

class TransformComponent
{
public:
    glm::vec3 position={0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
};


#endif //INSTRENDERER_TRANSFORMCOMPONENT_H
