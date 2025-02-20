
#ifndef INSTRENDERER_RENDERRESHA_H
#define INSTRENDERER_RENDERRESHA_H
#include "../../Common/Core/HandleAllocator.h"
#include <utility>
#include "RenderResource.h"
namespace RDG
{
    using Handle = uint32_t;
    using DescriptorHandle = uint32_t;
    using ResHandle = std::pair<Handle,DescriptorHandle>;

    class RenderResHA : public HandleAllocator
    {
    public:
        ResHandle AllocResHandle(ResourceType type);
    };

} // RDG

#endif //INSTRENDERER_RENDERRESHA_H
