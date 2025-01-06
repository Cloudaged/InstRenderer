
#include "VulkanContext.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::unique_ptr<VulkanContext> VulkanContext::Pcontext = nullptr;
void VulkanContext::Init(SDL_Window* window,SDL_Event* event)
{
    if(!Pcontext)
    {
        Pcontext.reset(new VulkanContext(window));
    }
    Pcontext->sdlEvent = event;
    int width,height;
    SDL_GetWindowSize(Pcontext.get()->sdlWindow,&width,&height);
    Pcontext->windowExtent = VkExtent2D {(uint32_t)width,(uint32_t)height};
    Pcontext.get()->InitVulkanBackend();
}

VulkanContext &VulkanContext::GetContext()
{
    if(!Pcontext)
    {
        throw std::runtime_error("the context isn't exist");
    }
    return *(Pcontext.get());
}


VulkanContext::VulkanContext(SDL_Window* window)
{
    sdlWindow = window;
}

void VulkanContext::InitVulkanBackend()
{
    CreateInstance();
    CreateSurface();
    PickupPhysicalDevice();
    CreateQueueAndDevice();
    CreateVMAAllocator();
    CreateCommandPool();
    CreateSwapchain();
    CreateDrawImage();
    CreateDescriptorPool();
    CreateTestGlobalDescriptorSetLayout();
}


void VulkanContext::CreateInstance()
{
    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "hello triangle";
    appinfo.pEngineName ="Instinct";
    appinfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appinfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appinfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appinfo;

    instanceInfo.enabledLayerCount =static_cast<uint32_t>(validationLayers.size());
    instanceInfo.ppEnabledLayerNames = validationLayers.data();

    unsigned int extensionCount =0;
    SDL_Vulkan_GetInstanceExtensions(sdlWindow,&extensionCount,nullptr);
    std::vector<const char*> sdlExtensions(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(sdlWindow,&extensionCount,sdlExtensions.data());
    sdlExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    instanceInfo.enabledExtensionCount = sdlExtensions.size();
    instanceInfo.ppEnabledExtensionNames = sdlExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    populateDebugMessengerCreateInfo(debugCreateInfo);
    //instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;


    if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    } else
    {
        std::cout<<"Instance Created successed!\n";
    }

    if(SetupDebugMessenger()!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }


}

void VulkanContext::CreateSurface()
{
    if(!SDL_Vulkan_CreateSurface(sdlWindow,instance,&surface))
    {
        throw std::runtime_error("fail to create window surface!");
    }
}

void VulkanContext::PickupPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
    if(deviceCount==0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data());



    for (auto& device:devices)
    {
        if(VulkanFuncs::IsDeviceSuitable(device))
        {
            gpu = device;
        }
    }
    if(gpu==VK_NULL_HANDLE)
    {
        gpu = devices[0];
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
    std::cout<<deviceProperties.deviceName<<"\n";

}

void VulkanContext::CreateQueueAndDevice()
{
    QueueFamilyIndices indices = VulkanFuncs::FindQueueFamilies(gpu);
    familyIndices = indices;

    float graphicQueuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamily = {indices.graphicFamily.value(),indices.presentFamily.value()};

    for (uint32_t queueFamily:uniqueQueueFamily)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &graphicQueuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures supportedFeatures{};
    vkGetPhysicalDeviceFeatures(gpu,&supportedFeatures);

    if (!supportedFeatures.samplerAnisotropy) {
        throw std::runtime_error("Anisotropic filtering is not supported by the physical device.");
    }

    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{};
    multiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
    multiviewFeatures.multiview = VK_TRUE;


    VkPhysicalDeviceBufferDeviceAddressFeatures addressFeatures{};
    addressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    addressFeatures.bufferDeviceAddress = VK_TRUE;
    addressFeatures.pNext = &multiviewFeatures;

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
    indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    indexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    indexingFeatures.runtimeDescriptorArray = VK_TRUE;
    indexingFeatures.pNext = &addressFeatures;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.features.samplerAnisotropy=VK_TRUE;
    features2.pNext = &indexingFeatures;

    VkPhysicalDeviceVulkan13Features device13Features{};
    device13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    device13Features.synchronization2 =VK_TRUE;
    device13Features.pNext = &features2;


    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.pNext = &device13Features;

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    if(vkCreateDevice(gpu,&createInfo,nullptr,&device)!=VK_SUCCESS)
    {
        throw std::runtime_error("fail to create logical device!");
    }

    //get the queue from device
    vkGetDeviceQueue(device,indices.graphicFamily.value(),0,&queues.graphicsQueue);
    vkGetDeviceQueue(device,indices.presentFamily.value(),0,&queues.presentQueue);
}

void VulkanContext::CreateVMAAllocator()
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = gpu;
    allocatorInfo.device = device;
    allocatorInfo.instance = instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    if(vmaCreateAllocator(&allocatorInfo,&allocator)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create allocator!");
    }
}


void VulkanContext::CreateSwapchain()
{

    SwapChainSupportDetails swapChainSupportDetails = VulkanFuncs::QuerySwapChainSupport(gpu);

    VkSurfaceFormatKHR surfaceFormat = VulkanFuncs::ChooseSwapSurfaceFormat(swapChainSupportDetails.formats,VK_FORMAT_B8G8R8A8_SRGB,VK_COLORSPACE_SRGB_NONLINEAR_KHR);
    VkPresentModeKHR presentMode = VulkanFuncs::ChooseSwapPresentMode(swapChainSupportDetails.presentModes,VK_PRESENT_MODE_MAILBOX_KHR);
    VkExtent2D extent = VulkanFuncs::ChooseSwapExtent(swapChainSupportDetails.capabilities);

    uint32_t imageCount = INFLIGHT_COUNT;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent =extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    QueueFamilyIndices indices = VulkanFuncs::FindQueueFamilies(gpu);
    uint32_t queueFamilyIndices[] = {indices.graphicFamily.value(),indices.presentFamily.value()};

    if(indices.graphicFamily !=indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if(vkCreateSwapchainKHR(device,&createInfo, nullptr,&swapchainData.swapchain)!=VK_SUCCESS)
    {
        throw std::runtime_error("fail to create swap chain!");
    }


    presentManager.presentFrames.resize(imageCount);
    presentManager.swapchainView.resize(imageCount);
    presentManager.swapchainImage.resize(imageCount);
    vkGetSwapchainImagesKHR(device,swapchainData.swapchain,&imageCount,presentManager.swapchainImage.data());


    for (auto& image:presentManager.swapchainImage)
    {
        auto cmd = BeginSingleTimeCommands();

        bufferAllocator.TransitionImage(cmd,image,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        EndSingleTimeCommands(cmd);
    }


    for (int i = 0; i < imageCount; ++i)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = presentManager.swapchainImage[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = surfaceFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &presentManager.swapchainView[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture vk_image view!");
        }
    }

    swapchainData.format = surfaceFormat.format;
    swapchainData.extent = extent;
    this->windowExtent = extent;
}

void VulkanContext::CreateDrawImage()
{
    VkExtent2D drawImageExtent =
            {
                    windowExtent.width,
                    windowExtent.height
            };

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


}

void VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.queueFamilyIndex = VulkanContext::GetContext().familyIndices.graphicFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if(vkCreateCommandPool(VulkanContext::GetContext().device,&poolInfo, nullptr,&VulkanContext::GetContext().cmdPool)!=VK_SUCCESS)
    {
        std::cout<<"Failed to create pool\n";
    }

    if(vkCreateCommandPool(VulkanContext::GetContext().device,&poolInfo, nullptr,&VulkanContext::GetContext().resourceCmdpool)!=VK_SUCCESS)
    {
        std::cout<<"Failed to create pool\n";
    }
}

void VulkanContext::DrawPrepare()
{
    //std::lock_guard<std::mutex> guard(Locker::Get().loadResourceMtx);

    vkWaitForFences(device, 1, &presentManager.presentFrames[presentManager.currentFrame].renderFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device,1,&presentManager.presentFrames[presentManager.currentFrame].renderFence);


    auto result = vkAcquireNextImageKHR(device, swapchainData.swapchain, 1000000000,
                          presentManager.presentFrames[presentManager.currentFrame].swapChainSemaphore, nullptr,
                          &presentManager.swapChainImageIndex);
    if(result==VK_ERROR_OUT_OF_DATE_KHR)
    {
        presentManager.RecreateSwapChain();
        isResize = true;
        return;
    }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkCommandBuffer cmd = presentManager.presentFrames[presentManager.currentFrame].cmd;
    vkResetCommandBuffer(cmd, 0);

}

void VulkanContext::Submit()
{
    std::lock_guard<std::mutex> guard(Locker::Get().queueMtx);
    VkCommandBuffer cmd = presentManager.presentFrames[presentManager.currentFrame].cmd;
    auto waitSemaphore = presentManager.presentFrames[presentManager.currentFrame].swapChainSemaphore;
    auto renderSemaphore = presentManager.presentFrames[presentManager.currentFrame].renderSemaphore;

    if(isResize)
    {
        isResize = false;
        return;
    }

    //*************** submit
    VkCommandBufferSubmitInfo cmdInfo{};
    cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmdInfo.pNext = nullptr;
    cmdInfo.commandBuffer = cmd;
    cmdInfo.deviceMask = 0;

    VkSemaphoreSubmitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.semaphore = waitSemaphore;
    waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
    waitInfo.deviceIndex = 0;
    waitInfo.value = 1;

    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.pNext = nullptr;
    signalInfo.semaphore = renderSemaphore;
    signalInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    signalInfo.deviceIndex = 0;
    signalInfo.value = 1;

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.pNext = nullptr;

    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitInfo;


    submitInfo.signalSemaphoreInfoCount =  1;
    submitInfo.pSignalSemaphoreInfos = &signalInfo;

    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdInfo;


    auto result = vkQueueSubmit2(queues.graphicsQueue, 1, &submitInfo,
                   presentManager.presentFrames[presentManager.currentFrame].renderFence);

    if(result!=VK_SUCCESS)
    {
        std::cout<<result<<"\n";
        throw std::runtime_error("failed to submit!");
    }
    //********************************
    //***************Present
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &swapchainData.swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &presentManager.presentFrames[presentManager.currentFrame].renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &presentManager.swapChainImageIndex;

    auto result1 = vkQueuePresentKHR(queues.graphicsQueue, &presentInfo);

    if(result==VK_ERROR_OUT_OF_DATE_KHR)
    {
        presentManager.RecreateSwapChain();
        isResize = true;
    }else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    presentManager.currentFrame = ++presentManager.currentFrame%INFLIGHT_COUNT;
}

void VulkanContext::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize,3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(100);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(200);
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(100);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT|VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(400);


    if(vkCreateDescriptorPool(VulkanContext::GetContext().device,&poolInfo, nullptr,&pool)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkCommandBuffer VulkanContext::BeginSingleTimeCommands(bool isResourceThread)
{

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(!isResourceThread)
    {
        allocInfo.commandPool =  cmdPool;
    } else
    {
        allocInfo.commandPool = resourceCmdpool;
    }

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(VulkanContext::GetContext().device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;

}

void VulkanContext::EndSingleTimeCommands(VkCommandBuffer commandBuffer,bool isResourceThread)
{
    std::lock_guard<std::mutex> guard(Locker::Get().queueMtx);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(VulkanContext::GetContext().queues.graphicsQueue,1,&submitInfo,VK_NULL_HANDLE);
    vkQueueWaitIdle(VulkanContext::GetContext().queues.graphicsQueue);


    if(!isResourceThread)
    {
        vkFreeCommandBuffers(VulkanContext::GetContext().device,
                             cmdPool,
                             1,&commandBuffer);
    } else
    {
        vkFreeCommandBuffers(VulkanContext::GetContext().device,
                             resourceCmdpool,
                             1,&commandBuffer);
    }

}

VkResult  VulkanContext::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, &createInfo, nullptr, &debugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanContext::CreateTestGlobalDescriptorSetLayout()
{
    std::array<VkDescriptorSetLayoutBinding, 3> bindings{};
    std::array<VkDescriptorBindingFlags, 3> flags{};
    std::array<VkDescriptorType, 3> types
    {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    };

    for (uint32_t i = 0; i < 3; ++i)
    {
        bindings.at(i).binding = i;
        bindings.at(i).descriptorType = types.at(i);
        bindings.at(i).descriptorCount = 1000;
        bindings.at(i).stageFlags = VK_SHADER_STAGE_ALL;
        flags.at(i) = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
    bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlags.pNext = nullptr;
    bindingFlags.pBindingFlags = flags.data();
    bindingFlags.bindingCount = 3;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = 3;
    layoutCreateInfo.pBindings = bindings.data();
    layoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layoutCreateInfo.pNext = &bindingFlags;

    vkCreateDescriptorSetLayout(device,&layoutCreateInfo, nullptr,&bindlessLayout);

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = pool;
    allocateInfo.pSetLayouts = &bindlessLayout;
    allocateInfo.descriptorSetCount = 1;

    vkAllocateDescriptorSets(device, &allocateInfo, &bindlessSet);

}


