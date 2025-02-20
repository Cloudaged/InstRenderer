
#include "Locker.h"
std::unique_ptr<Locker> Locker::Pinstance = nullptr;

void Locker::Init()
{
    if(!Pinstance)
    {
        Pinstance.reset(new Locker());
    }
}

Locker &Locker::Get()
{
    if(!Pinstance)
    {
        throw std::runtime_error("the context isn't exist");
    }
    return *(Pinstance.get());
}

Locker::Locker()
{

}
