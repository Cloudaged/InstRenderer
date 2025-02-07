
#include "EngineMath.h"
namespace EngineMath
{
    glm::mat4 GetModelMatrix(const Transform& trans)
    {
        auto mat = glm::translate(glm::mat4(1),trans.pos);
        mat = glm::scale(mat,trans.scale);
        mat =  GetRotateMatrix(trans.rotation,mat);
        return mat;
    }

    VkTransformMatrixKHR GlmToVkTransform(const glm::mat4& mat)
    {
        VkTransformMatrixKHR transform{};
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                transform.matrix[i][j] = mat[j][i];
            }
        }
        return transform;
    }


    glm::mat4 GetRotateMatrix(const glm::vec3& rotationAngle,glm::mat4 mat)
    {
        mat = glm::rotate(mat,glm::radians(rotationAngle.x),{1,0,0});
        mat = glm::rotate(mat,glm::radians(rotationAngle.y),{0,1,0});
        mat = glm::rotate(mat,glm::radians(rotationAngle.z),{0,0,1});
        return mat;
    }

    glm::vec3 SphereToRectCoord(const glm::vec3& angle,const float& r)
    {
        float x = r * sin(angle.y) * cos(angle.z);
        float z = r * sin(angle.y) * sin(angle.z);
        float y = r * cos(angle.y);
        return {x,y,z};
    }

    MaxMinPoints TransformAABB(const BoundingPoint& min,const BoundingPoint& max,const glm::mat4 mat)
    {
        std::vector<BoundingPoint> points{
                {min.x,min.y,min.z},
                {max.x,min.y,min.z},
                {min.x,max.y,min.z},
                {max.x,max.y,min.z},
                {min.x,min.y,max.z},
                {max.x,min.y,max.z},
                {min.x,max.y,max.z},
                {max.x,max.y,max.z}
        };
        BoundingPoint newMax = glm::vec3(std::numeric_limits<float>::lowest());
        BoundingPoint newMin = glm::vec3(std::numeric_limits<float>::max());

        for (auto& point:points)
        {
            point = glm::vec3(mat*glm::vec4(point,1.0));
            newMax = glm::max(newMax,point);
            newMin = glm::min(newMin,point);
        }
        return {newMax,newMin};
    }

    MaxMinPoints GetFrustumBoundingBox(Camera& camera)
    {
        const auto& data = camera.cameraData;

        float nearHeight = tan(data.fov/2) * data.nearPlane;
        float nearWidth = nearHeight * data.aspect;
        float farHeight = tan(data.fov/2) * data.farPlane;
        float farWidth = farHeight * data.aspect;

        std::array<BoundingPoint,8> frustum;
        frustum[0] = {nearWidth,nearHeight,data.nearPlane};
        frustum[1] = {-nearWidth,nearHeight,data.nearPlane};
        frustum[2] = {nearWidth,-nearHeight,data.nearPlane};
        frustum[3] = {-nearWidth,-nearHeight,data.nearPlane};

        frustum[4] = {farWidth,farHeight,data.farPlane};
        frustum[5] = {-farWidth,farHeight,data.farPlane};
        frustum[6] = {farWidth,-farHeight,data.farPlane};
        frustum[7] = {-farWidth,-farHeight,data.farPlane};

        const auto& camMat = camera.vpMat.view;
        auto inverseCamMat = glm::inverse(camMat);

        BoundingPoint min = glm::vec3 (std::numeric_limits<float>::max());
        BoundingPoint max = glm::vec3 (std::numeric_limits<float>::lowest());
        for (auto& point : frustum)
        {
            auto result = glm::vec3 (inverseCamMat * glm::vec4(point,1.0));
            min = glm::min(result,min);
            max = glm::max(result,max);
        }

        return {max,min};
    }

    Circumsphere GetFrustumCircumsphere(const std::vector<glm::vec3>& corners)
    {
        Circumsphere circumsphere;

        float a2 = pow(glm::length(corners[3]-corners[1]),2);
        float b2 = pow(glm::length(corners[7]-corners[5]),2);
        glm::vec3 nearCenter = (corners[0]+corners[2])/2.0f;
        glm::vec3 farCenter = (corners[4]+corners[6])/2.0f;
        auto length = glm::distance(nearCenter,farCenter);
        float offset = (length/2)+(a2-b2)/(8*length);
        glm::vec3 centerLineDir = glm::normalize(farCenter-nearCenter);
        circumsphere.center = nearCenter-offset*centerLineDir;
        circumsphere.radius = glm::length(circumsphere.center-corners[0]);

        return circumsphere;
    }


}