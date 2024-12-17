
#ifndef INSTRENDERER_PASSREF_H
#define INSTRENDERER_PASSREF_H
#include "vulkan/vulkan.h"
#include "vector"
#include "string"
namespace RDG
{
    struct PassRef
    {
        int data;
        std::vector<std::string> producer;
    };

} // RDG

#endif //INSTRENDERER_PASSREF_H
