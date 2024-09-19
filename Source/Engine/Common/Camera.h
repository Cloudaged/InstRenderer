
#ifndef INSTRENDERER_CAMERA_H
#define INSTRENDERER_CAMERA_H
#include "glm/glm.hpp"
#include "GameObject.h"
#include "../Common/Component/Components.h"

#include "glm/gtc/matrix_transform.hpp"
class Camera:public GameObject
{
public:
    Camera(entt::registry* reg,std::string name);

    glm::mat4 GetViewMatrix(entt::registry reg);

    float pitch=0;
    float yaw = 90;

    void InitCamera(glm::vec3 Position, glm::vec3 Target, glm::vec3 UpDir);
    glm::vec3 viewPoint;
    glm::vec3 position;
    glm::vec3 upDir;

    glm::vec3 xAxis,yAxis,zAxis;

    struct VPMat
    {
        glm::mat4 view;
        glm::mat4 proj;
    }vpMat;
};


#endif //INSTRENDERER_CAMERA_H
