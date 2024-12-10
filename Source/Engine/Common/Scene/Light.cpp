
#include "Light.h"
#include "../Core/EngineMath.h"
Light::Light(entt::registry *reg, std::string name,std::string type) : GameObject(reg, name,type)
{
    reg->emplace<LightComponent>(this->entityID,LightType::Directional,glm::vec3{1.0,1.0,1.0},1,1,0,0,ShadowCamera{1.0,1000.0f});
    this->componentBits.set((int)ComponentType::LightComponent);
}

LightVPMat Light::GetLightMatrix(entt::registry *reg,const glm::vec3& minPoint,const glm::vec3& maxPoint)
{
    const Transform& transform = reg->get<Transform>(this->entityID);
    auto& lightComp = reg->get<LightComponent>(this->entityID);
    if(lightComp.type == LightType::Directional)
    {
        auto rotationMat = EngineMath::GetRotateMatrix(transform.rotation);
        glm::vec4 target = rotationMat*glm::vec4(0.0,0.0,1.0,0.0);
        glm::vec3 sceneCenter = (minPoint + maxPoint) / 2.0f;
        float maxDepth = glm::distance(maxPoint,minPoint);
        glm::vec3 position = sceneCenter - glm::vec3(target)*maxDepth;
        glm::mat4 lightMat = glm::lookAt(position,sceneCenter,{0,1,0});
        auto [sceneMaxLS,sceneMinLS] = EngineMath::TransformAABB(minPoint,maxPoint,lightMat);//world space to light space
        glm::mat4 projMat = glm::ortho(sceneMinLS.x-100,sceneMaxLS.x+100,sceneMinLS.y-100,sceneMaxLS.y+100,1.0f+lightComp.shadowCamera.near,maxDepth+lightComp.shadowCamera.far);
        projMat[1][1] *= -1;
        return {lightMat,projMat};
    }else if(lightComp.type == LightType::Point)
    {

    }else if(lightComp.type == LightType::Spot)
    {
        auto rotationMat = EngineMath::GetRotateMatrix(transform.rotation);
        glm::vec4 target = rotationMat*glm::vec4(0.0,0.0,1.0,0.0);
        glm::mat4 lightMat = glm::lookAt(transform.pos,glm::vec3(target)+transform.pos,{0,1,0});
        glm::mat4 projMat = glm::perspective(glm::radians(lightComp.range*2),1.0f,100.0f,5000.0f);
        projMat[1][1] *=-1;
        return {lightMat,projMat};
    }
    return LightVPMat();
}
