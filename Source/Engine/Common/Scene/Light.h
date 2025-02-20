
#ifndef INSTRENDERER_LIGHT_H
#define INSTRENDERER_LIGHT_H
#include "GameObject.h"
#include "../Component/Components.h"

struct LightVPMat
{
    glm::mat4 v;
    glm::mat4 p;
};

class Light:public GameObject
{
public:
    Light(entt::registry* reg,std::string name,std::string type);
    LightVPMat GetLightMatrix(entt::registry* reg,const glm::vec3& minPoint,const glm::vec3& maxPoint);
    LightVPMat GetSubFrustumLightMatrix(entt::registry* reg,const glm::vec3& frustumCenter,const float& radius,const glm::vec3& minPoint,const glm::vec3& maxPoint);
};


#endif //INSTRENDERER_LIGHT_H
