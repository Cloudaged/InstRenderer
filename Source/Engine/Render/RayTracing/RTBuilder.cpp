
#include "RTBuilder.h"
#include "../VulkanContext.h"

RTScene RTBuilder::CreateRTScene(entt::view<entt::get_t<Renderable,Transform>> view)
{
    RTScene rtScene;
    if(view.size_hint()<=0)
    {
        rtScene.allBlas.push_back(CreateEmptyBLAS());
    } else
    {
        rtScene.allBlas = CreateBLAS(view);
    }
    rtScene.topAS = CreateTLAS(rtScene.allBlas);
    return rtScene;
}

std::vector<BLAS> RTBuilder::CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view)
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

        range.primitiveCount = mesh.indexCount/3;

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
    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    for (auto& entity : view)
    {
        auto& renderable = view.get<Renderable>(entity);
        auto& mesh = renderable.mesh;
        allBlas[index].materialHandle = renderable.materialID;
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
    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);

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

std::vector<Vertex> nVertices = {
        // 第一个顶点
        {
                glm::vec3( 0.0f,  1.0f,  0.0f),    // position
                glm::vec3( 0.0f,  0.0f,  1.0f),    // normal (假设法线朝向屏幕)
                glm::vec2( 0.5f,  1.0f),            // uv
                glm::vec4( 1.0f,  0.0f,  0.0f, 1.0f), // tangent
                glm::vec3( 0.0f,  1.0f,  0.0f)     // bitangent
        },
        // 第二个顶点
        {
                glm::vec3(-1.0f, -1.0f,  0.0f),    // position
                glm::vec3( 0.0f,  0.0f,  1.0f),    // normal
                glm::vec2( 0.0f,  0.0f),            // uv
                glm::vec4( 1.0f,  0.0f,  0.0f, 1.0f), // tangent
                glm::vec3( 0.0f,  1.0f,  0.0f)     // bitangent
        },
        // 第三个顶点
        {
                glm::vec3( 1.0f, -1.0f,  0.0f),    // position
                glm::vec3( 0.0f,  0.0f,  1.0f),    // normal
                glm::vec2( 1.0f,  0.0f),            // uv
                glm::vec4( 1.0f,  0.0f,  0.0f, 1.0f), // tangent
                glm::vec3( 0.0f,  1.0f,  0.0f)     // bitangent
        }
};

// 三角形的索引
std::vector<uint32_t> nIndices = {
        0, 1, 2  // 定义三角形的三个顶点
};


BLAS RTBuilder::CreateEmptyBLAS()
{
    auto device = VulkanContext::GetContext().device;
    size_t meshCount =1;
    VkAccelerationStructureGeometryKHR geometry{};
    VkAccelerationStructureBuildRangeInfoKHR range{};
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};

    VkTransformMatrixKHR transformMatrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
    };

    Buffer* transBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(VkTransformMatrixKHR),
                                                                              VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
                                                                              VMA_MEMORY_USAGE_CPU_TO_GPU);
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*transBuffer),&transformMatrix,sizeof(VkTransformMatrixKHR));

    VkBufferDeviceAddressInfo deviceAddressInfoT{};
    deviceAddressInfoT.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    deviceAddressInfoT.buffer = transBuffer->vk_buffer;
    auto transAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfoT);


    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    int index = 0;


    Mesh mesh(nVertices,nIndices);


    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

    range.primitiveCount = mesh.indexCount/3;
    range.primitiveOffset = 0;
    range.firstVertex = 0;
    range.transformOffset = 0;

    geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    geometry.geometry.triangles.vertexData.deviceAddress = mesh.vertAddress;
    geometry.geometry.triangles.vertexStride = sizeof(Vertex);
    geometry.geometry.triangles.maxVertex = mesh.vertexCount;
    geometry.geometry.triangles.indexData.deviceAddress = mesh.indexAddress;
    geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
    geometry.geometry.triangles.transformData.deviceAddress = transAddress;

    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &geometry;


    vkGetAccelerationStructureBuildSizesKHR(device,
                                            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                            &buildInfo,
                                            &range.primitiveCount,
                                            &sizeInfo);
    index = 0;
    VkDeviceSize maxBlasSize = 0;

    maxBlasSize = std::max(maxBlasSize,sizeInfo.buildScratchSize);


    Buffer* scratchBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(maxBlasSize,
                                                                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                                     VMA_MEMORY_USAGE_GPU_ONLY);

    BLAS blas;
    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    blas.asBuffer =  VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeInfo.accelerationStructureSize,
                                                                                        VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR|
                                                                                        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    VkAccelerationStructureCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    createInfo.size =  sizeInfo.accelerationStructureSize;
    createInfo.buffer = blas.asBuffer->vk_buffer;

    if(vkCreateAccelerationStructureKHR(device,&createInfo, nullptr,&blas.accelerationStructure)!=VK_SUCCESS)
    {
        std::cout<<"Failed to Create BLAS\n";
    }
    VkBufferDeviceAddressInfo deviceAddressInfo{};
    deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    deviceAddressInfo.buffer = scratchBuffer->vk_buffer;

    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.scratchData.deviceAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfo);
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.dstAccelerationStructure = blas.accelerationStructure;

    const VkAccelerationStructureBuildRangeInfoKHR* crange[1] = { &range };

    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, crange);

    // guard our scratch buffer
    vkCmdPipelineBarrier(cmd,
                         VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                         VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                         0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);

    VulkanContext::GetContext().bufferAllocator.DestroyBuffer(*scratchBuffer);

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = blas.accelerationStructure;
    blas.address = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);
    return blas;
}

TLAS RTBuilder::CreateTLAS(const std::vector<BLAS>& allblas)
{
    auto& device = VulkanContext::GetContext().device;

    const VkTransformMatrixKHR transform =
            {
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
            };

    size_t blasCount = allblas.size();
    std::vector<VkAccelerationStructureInstanceKHR> instances(blasCount,VkAccelerationStructureInstanceKHR{});
    VkAccelerationStructureInstanceKHR emptyInstance{};

    if(blasCount==0)
    {
        blasCount++;
        emptyInstance.transform = transform;
        emptyInstance.instanceCustomIndex = 0;
        emptyInstance.mask = 0xFF;
        emptyInstance.instanceShaderBindingTableRecordOffset = 0;
        emptyInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        emptyInstance.accelerationStructureReference = 0;
        std::cout<<"Empty is disable now\n";
        instances.push_back(emptyInstance);
    }else
    {
        for (int i = 0; i < blasCount; ++i)
        {
            auto& blas = allblas[i];
            auto& instance = instances[i];
            instance.transform = transform;
            instance.instanceCustomIndex = blas.materialHandle;
            instance.mask = 0xFF;
            instance.instanceShaderBindingTableRecordOffset = 0;
            instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            instance.accelerationStructureReference = blas.address;
        }
    }

    Buffer* instancesBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(instances.size() * sizeof(VkAccelerationStructureInstanceKHR),
                                                                                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
                                                                                       VMA_MEMORY_USAGE_CPU_TO_GPU);
    void* instancesMem = VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*instancesBuffer);
    memcpy(instancesMem,instances.data(),instances.size() * sizeof(VkAccelerationStructureInstanceKHR));
    //memcpy(instances.data(),instancesMem,instances.size() * sizeof(VkAccelerationStructureInstanceKHR));

    VkBufferDeviceAddressInfo instanceAddressInfo{};
    instanceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    instanceAddressInfo.buffer = instancesBuffer->vk_buffer;

    VkAccelerationStructureGeometryKHR  tlasGeoInfo = {};
    tlasGeoInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    tlasGeoInfo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    tlasGeoInfo.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    tlasGeoInfo.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    tlasGeoInfo.geometry.instances.arrayOfPointers = VK_FALSE;
    tlasGeoInfo.geometry.instances.data.deviceAddress = vkGetBufferDeviceAddress(device,&instanceAddressInfo);

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &tlasGeoInfo;

    uint32_t instancesCount = instances.size();
    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR};
    vkGetAccelerationStructureBuildSizesKHR(device,VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,&buildInfo, &instancesCount, &sizeInfo);

    TLAS tlas;
    tlas.asBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeInfo.accelerationStructureSize,
                                                                                  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                                                                  VMA_MEMORY_USAGE_GPU_ONLY);

    VkAccelerationStructureCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    createInfo.size = sizeInfo.accelerationStructureSize;
    createInfo.buffer = tlas.asBuffer->vk_buffer;

    if(vkCreateAccelerationStructureKHR(device,&createInfo, nullptr,&tlas.accelerationStructure)!=VK_SUCCESS)
    {
        std::cout<<"Failed to create alts\n";
    }

    Buffer* scratchBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeInfo.accelerationStructureSize,
                                                                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                                     VMA_MEMORY_USAGE_GPU_ONLY);
    VkBufferDeviceAddressInfo scratchAddress{};
    scratchAddress.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    scratchAddress.buffer = scratchBuffer->vk_buffer;

    buildInfo.scratchData.deviceAddress = vkGetBufferDeviceAddress(device,&scratchAddress);
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.dstAccelerationStructure = tlas.accelerationStructure;

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands();
    VkAccelerationStructureBuildRangeInfoKHR range = {};
    range.primitiveCount = instancesCount;
    range.primitiveOffset = 0;
    range.firstVertex = 0;
    range.transformOffset = 0;

    const VkAccelerationStructureBuildRangeInfoKHR* ranges[1] = {&range} ;

    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, ranges);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd);


    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = tlas.accelerationStructure;
    tlas.address = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);
    return tlas;
}




