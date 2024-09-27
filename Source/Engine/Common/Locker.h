
#ifndef INSTRENDERER_LOCKER_H
#define INSTRENDERER_LOCKER_H
#include "mutex"
#include "memory"

class Locker
{
public:
    static Locker& Get();
    static void Init();
    std::mutex resizeMtx;
private:
    Locker();
    static std::unique_ptr<Locker> Pinstance;

};
#endif //INSTRENDERER_LOCKER_H
