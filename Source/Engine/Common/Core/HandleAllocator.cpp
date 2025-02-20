
#include "HandleAllocator.h"

Handle HandleAllocator::Allocate()
{
    if(!reusableHandle.empty())
    {
        Handle handle = reusableHandle.front();
        reusableHandle.pop();
        return handle;
    }
    return nextHandle++;
}

void HandleAllocator::Deallocate(Handle handle)
{
    if (handle > 0 && handle < nextHandle)
    {
        reusableHandle.push(handle);
    }
}
