
#ifndef INSTRENDERER_ENGINEMATH_H
#define INSTRENDERER_ENGINEMATH_H
#include "../Core/glmConfig.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../Component/Components.h"
#include "../Scene/Camera.h"
#include <array>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

using BoundingPoint = glm::vec3;

namespace EngineMath
{
    struct MaxMinPoints
    {
        BoundingPoint max;
        BoundingPoint min;
    };
    struct Circumsphere
    {
        glm::vec3 center;
        float radius;
    };
    glm::mat4 GetModelMatrix(const Transform& trans);
    glm::mat4 GetRotateMatrix(const glm::vec3& rotationAngle,glm::mat4 mat = glm::mat4(1.0f));
    glm::vec3 SphereToRectCoord(const glm::vec3& angle,const float& r);
    MaxMinPoints TransformAABB(const BoundingPoint& min,const BoundingPoint& max,const glm::mat4 mat);
    MaxMinPoints UpdateFrustumBoundingBox(Camera& camera);
    Circumsphere GetFrustumCircumsphere(const std::vector<glm::vec3>& corners,float length);
}

#endif //INSTRENDERER_ENGINEMATH_H
