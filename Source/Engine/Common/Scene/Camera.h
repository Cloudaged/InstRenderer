
#ifndef INSTRENDERER_CAMERA_H
#define INSTRENDERER_CAMERA_H
#include "../Core/glmConfig.h"
#include "GameObject.h"
#include "../Component/Components.h"
#include <array>
#include "glm/gtc/matrix_transform.hpp"
#include "SceneSubject.h"
using BoundingPoint = glm::vec3;


struct CameraData
{
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
};

class Camera:public GameObject,public SceneSubject
{
public:
    Camera(entt::registry* reg,std::string name);
    void InitCamera(glm::vec3 Position, glm::vec3 Target, glm::vec3 UpDir);
    glm::mat4 GetViewMatrix(entt::registry reg);
    glm::vec3 GetCameraPos();
    void SetCameraPos(glm::vec3 newPos);
    glm::vec3 GetCameraTarget();
    void SetCameraTarget(glm::vec3 newTarget);
    glm::vec3 GetCameraUpDir();
    void Update();
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
                1,5.0f,5000.0f
            };
};


#endif //INSTRENDERER_CAMERA_H
