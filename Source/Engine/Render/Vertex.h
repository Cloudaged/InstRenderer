
#ifndef INSTRENDERER_VERTEX_H
#define INSTRENDERER_VERTEX_H
#include "glm/glm.hpp"
#include "vector"
#include "vulkan/vulkan.h"
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 tangent;
    glm::vec3 bitangent;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindDescription{};
        bindDescription.binding = 0;
        bindDescription.stride = sizeof(Vertex);
        bindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(5);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex,position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex,normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex,uv);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex,tangent);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex,bitangent);

        return attributeDescriptions;

    }
};



#endif //INSTRENDERER_VERTEX_H
