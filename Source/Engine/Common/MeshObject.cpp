
#include "MeshObject.h"

MeshObject::MeshObject(entt::registry* reg, std::string name) : GameObject(reg, name)
{
    reg->emplace<Transform>(entityID,glm::vec3{0,0,0},glm::vec3{0,0,0},glm::vec3{1,1,1});
    reg->emplace<Renderable>(entityID,
                             Mesh(std::vector<Vertex>(0),std::vector<uint32_t>(0)),
                             PBRMaterial(),
                             glm::vec3(0,0,0)
                             );
}
