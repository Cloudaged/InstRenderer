
#include "RayTracingBuilder.h"
#include "../VulkanContext.h"

std::vector<BLAS> RayTracingBuilder::CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view)
{
    size_t meshCount = view.size_hint();
    std::vector<VkAccelerationStructureGeometryKHR> geometries(meshCount,VkAccelerationStructureGeometryKHR{});
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges(meshCount,VkAccelerationStructureBuildRangeInfoKHR{});
    std::vector<VkAccelerationStructureBuildGeometryInfoKHR> buildInfos(meshCount, VkAccelerationStructureBuildGeometryInfoKHR{});

    std::vector<VkAccelerationStructureBuildSizesInfoKHR> sizeInfos(meshCount, VkAccelerationStructureBuildSizesInfoKHR{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR });
    int index = 0;

    auto& device = VulkanContext::GetContext().device;
    for (auto& entity:view)
    {
        auto& renderable = view.get<Renderable>(entity);

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


        vkGetAccelerationStructureBuildSizesKHR(device,
                                                VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                                &buildInfo,
                                                &range.primitiveCount,
                                                &sizeInfos[index]);
        index++;
    }
    index = 0;
    VkDeviceSize maxBlasSize = 0;
    for (const auto& sizeInfo : sizeInfos)
    {
        maxBlasSize = std::max(maxBlasSize,sizeInfo.buildScratchSize);
    }

    Buffer* scratchBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(maxBlasSize,
                                                                                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                                    VMA_MEMORY_USAGE_GPU_ONLY);

    std::vector<BLAS> allBlas(meshCount);
    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands();

    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    for (auto& entity : view)
    {
        auto& renderable = view.get<Renderable>(entity);
        auto& mesh = renderable.mesh;
        allBlas[index].asBuffer =  VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeInfos[index].accelerationStructureSize,
                                                                                            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR|
                                                                                            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
        VkAccelerationStructureCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        createInfo.size =  sizeInfos[index].accelerationStructureSize;
        createInfo.buffer = allBlas[index].asBuffer->vk_buffer;

        if(vkCreateAccelerationStructureKHR(device,&createInfo, nullptr,&allBlas[index].accelerationStructure)!=VK_SUCCESS)
        {
            std::cout<<"Failed to Create BLAS\n";
        }

        VkAccelerationStructureBuildGeometryInfoKHR buildInfo = buildInfos[index];

        VkBufferDeviceAddressInfo deviceAddressInfo{};
        deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        deviceAddressInfo.buffer = scratchBuffer->vk_buffer;

        buildInfo.scratchData.deviceAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfo);
        buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
        buildInfo.dstAccelerationStructure = allBlas[index].accelerationStructure;

        const VkAccelerationStructureBuildRangeInfoKHR* range[1] = { &ranges[index] };

        vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, range);

        // guard our scratch buffer
        vkCmdPipelineBarrier(cmd,
                             VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                             VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                             0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
        index++;
    }
    VulkanContext::GetContext().EndSingleTimeCommands(cmd);

    VulkanContext::GetContext().bufferAllocator.DestroyBuffer(*scratchBuffer);
    index = 0;
    for (auto& entity:view)
    {
        auto& renderable = view.get<Renderable>(entity);
        auto& mesh = renderable.mesh;

        VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = allBlas[index].accelerationStructure;
        allBlas[index].address = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);

        index++;
    }


    return allBlas;
}

TLAS RayTracingBuilder::CreateTLAS(VkCommandBuffer cmd)
{
    VkAccelerationStructureGeometryKHR geometry{};

    return TLAS();
}


