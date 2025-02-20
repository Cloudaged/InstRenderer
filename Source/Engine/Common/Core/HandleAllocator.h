
#ifndef INSTRENDERER_HANDLEALLOCATOR_H
#define INSTRENDERER_HANDLEALLOCATOR_H

#include <queue>
#include <vector>
#include "volk.h"
using Handle = uint32_t;

class HandleAllocator
{
public:
    Handle Allocate();
    void Deallocate(Handle handle);
private:
    std::queue<Handle> reusableHandle;
    Handle nextHandle=1;
};


#endif //INSTRENDERER_HANDLEALLOCATOR_H
