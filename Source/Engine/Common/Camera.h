
#ifndef INSTRENDERER_CAMERA_H
#define INSTRENDERER_CAMERA_H
#include "glm/glm.hpp"
#include "GameObject.h"
#include "../Common/Component/Components.h"
#include <array>
#include "glm/gtc/matrix_transform.hpp"
using BoundingPoint = glm::vec3;


struct CameraData
{
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
};

class Camera:public GameObject
{
public:
    Camera(entt::registry* reg,std::string name);
    glm::mat4 GetViewMatrix(entt::registry reg);
    void ResetViewMatrix();
    glm::vec3 GetCameraPos();
    void SetCameraPos(glm::vec3 newPos);
    glm::vec3 GetCameraTarget();
    void SetCameraTarget(glm::vec3 newTarget);
    glm::vec3 GetCameraUpDir();
    void InitCamera(glm::vec3 Position, glm::vec3 Target, glm::vec3 UpDir);
    void UpdateAspect();
public:
    float pitch=0;
    float yaw = 90;
    glm::vec3 viewPoint;
    glm::vec3 position;
    glm::vec3 upDir;
    glm::vec3 xAxis,yAxis,zAxis;
    struct VPMat
    {
        glm::mat4 view;
        glm::mat4 proj;
    }vpMat;
    CameraData cameraData =
            CameraData{
                glm::radians(80.0f),
                1,5.0f,20000.0f
            };
};


#endif //INSTRENDERER_CAMERA_H
