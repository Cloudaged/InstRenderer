
#include "EngineMath.h"
namespace EngineMath
{
    glm::mat4 GetModelMatrix(const Transform& trans)
    {
        auto mat =  glm::translate(glm::mat4(1),trans.pos);
        mat = glm::scale(mat,trans.scale);
        mat =  GetRotateMatrix(trans.rotation,mat);
        return mat;
    }

    glm::mat4 GetRotateMatrix(const glm::vec3& rotationAngle,glm::mat4 mat)
    {
        mat = glm::rotate(mat,rotationAngle.x,{1,0,0});
        mat = glm::rotate(mat,rotationAngle.y,{0,1,0});
        mat = glm::rotate(mat,rotationAngle.z,{0,0,1});
        return mat;
    }

}