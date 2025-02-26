
#include "Camera.h"
#include "../../Render/VulkanContext.h"

Camera::Camera(entt::registry* reg,std::string name):
GameObject(reg,name)
{


}

void Camera::InitCamera(glm::vec3 Position, glm::vec3 Target, glm::vec3 UpDir)
{
    position = Position;
    viewPoint = Target;
    upDir = UpDir;
    rightDir = glm::cross(UpDir,Target);
    this->vpMat.view = glm::lookAt(position,
                                   viewPoint + position,upDir);

    this->vpMat.proj = glm::perspective(cameraData.fov,
                                        (float)VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                        cameraData.nearPlane, cameraData.farPlane);
    this->vpMat.proj[1][1] *=-1;

}


glm::mat4 Camera::GetViewMatrix(entt::registry reg)
{
    return vpMat.view;
}

void Camera::SetCameraPos(glm::vec3 newPos)
{
    this->position = newPos;
    Update();
}

glm::vec3 Camera::GetCameraPos()
{
    return this->position;
}

glm::vec3 Camera::GetCameraTarget()
{
    return this->viewPoint;
}

void Camera::SetCameraTarget(glm::vec3 newTarget)
{
    this->viewPoint = newTarget;
    this->rightDir = glm::cross(upDir,newTarget);
    Update();
}

glm::vec3 Camera::GetCameraUpDir()
{
    return this->upDir;
}


void Camera::Update()
{
    NotifyCamera(*this);
}

void Camera::UpdateAspect()
{

}

