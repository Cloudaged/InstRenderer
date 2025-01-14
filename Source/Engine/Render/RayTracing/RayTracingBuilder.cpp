
#include "RayTracingBuilder.h"
#include "../VulkanContext.h"

std::vector<BLAS> RayTracingBuilder::CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view,VkCommandBuffer cmd)
{
    size_t meshCount = view.size_hint();
    std::vector<VkAccelerationStructureGeometryKHR> geometries(meshCount,VkAccelerationStructureGeometryKHR{});
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges(meshCount,VkAccelerationStructureBuildRangeInfoKHR{});
    std::vector<VkAccelerationStructureBuildGeometryInfoKHR> buildInfos(meshCount, VkAccelerationStructureBuildGeometryInfoKHR{});

    std::vector<VkAccelerationStructureBuildSizesInfoKHR> sizeInfos(meshCount, VkAccelerationStructureBuildSizesInfoKHR{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR });
    int index = 0;
    for (auto& entity:view)
    {
        auto& renderable = view.get<Renderable>(entity);
        auto& transform = view.get<Transform>(entity);

        auto& mesh = renderable.mesh;
        auto& geometry = geometries[index];
        auto& range = ranges[index];
        auto& buildInfo = buildInfos[index];

        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

        geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometry.geometry.triangles.vertexData.deviceAddress = mesh.vertAddress;
        geometry.geometry.triangles.vertexStride = sizeof(Vertex);
        geometry.geometry.triangles.maxVertex = mesh.vertexCount;
        geometry.geometry.triangles.indexData.deviceAddress = mesh.indexAddress;
        geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;

        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &geometry;


        vkGetAccelerationStructureBuildSizesKHR(VulkanContext::GetContext().device,
                                                VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                                &buildInfo,
                                                &range.primitiveCount,
                                                &sizeInfos[index]);
        index++;
    }

    VkDeviceSize maxBlasSize = 0;
    for (const auto& sizeInfo : sizeInfos)
    {
        maxBlasSize = std::max(maxBlasSize,sizeInfo.buildScratchSize);
    }

//    Buffer scratchBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(maxBlasSize,
//                                                                                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
//                                                                                    VMA_MEMORY_USAGE_GPU_ONLY);


    return std::vector<BLAS>(0);
}

TLAS RayTracingBuilder::CreateTLAS(VkCommandBuffer cmd)
{
    VkAccelerationStructureGeometryKHR geometry{};

    return TLAS();
}


