
#ifndef INSTRENDERER_ENGINEMATH_H
#define INSTRENDERER_ENGINEMATH_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Component/Components.h"
namespace EngineMath
{
    glm::mat4 GetModelMatrix(const Transform& trans);
    glm::mat4 GetRotateMatrix(const glm::vec3& rotationAngle,glm::mat4 mat = glm::mat4(1.0f));
}

#endif //INSTRENDERER_ENGINEMATH_H
