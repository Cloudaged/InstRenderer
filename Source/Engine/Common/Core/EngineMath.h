
#ifndef INSTRENDERER_ENGINEMATH_H
#define INSTRENDERER_ENGINEMATH_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Component/Components.h"
#include "../Camera.h"
#include <array>
using BoundingPoint = glm::vec3;

namespace EngineMath
{
    struct MaxMinPoints
    {
        BoundingPoint max;
        BoundingPoint min;
    };
    glm::mat4 GetModelMatrix(const Transform& trans);
    glm::mat4 GetRotateMatrix(const glm::vec3& rotationAngle,glm::mat4 mat = glm::mat4(1.0f));
    glm::vec3 SphereToRectCoord(const glm::vec3& angle,const float& r);
    MaxMinPoints TransformAABB(const BoundingPoint& min,const BoundingPoint& max,const glm::mat4 mat);
    MaxMinPoints GetFrustumBoundingBox(Camera& camera);
}

#endif //INSTRENDERER_ENGINEMATH_H
