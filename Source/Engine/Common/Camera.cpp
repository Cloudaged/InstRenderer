
#include "Camera.h"
#include "../Render/VulkanContext.h"

Camera::Camera(entt::entity id,std::string name,glm::vec3 Position, glm::vec3 Target, glm::vec3 UpDir, float Aspect):
GameObject(id,name),viewPoint(Target),position(Position),upDir(UpDir)
{

    this->zAxis = glm::normalize(Target-Position);
    this->yAxis = glm::normalize(UpDir);
    this->xAxis = glm::cross(yAxis,zAxis);



    this->vpMat.view = glm::lookAt(Position,
                                   viewPoint,yAxis);

    this->vpMat.proj = glm::perspective(glm::radians(80.0f),
                                        VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                        5.0f, 2000.0f);
    this->vpMat.proj[1][1] *=-1;

}

void Camera::InitCamera()
{

}

glm::mat4 Camera::GetViewMatrix(entt::registry reg)
{
    return vpMat.view;
}
