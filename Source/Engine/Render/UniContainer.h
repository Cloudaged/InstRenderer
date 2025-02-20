
#ifndef INSTRENDERER_UNICONTAINER_H
#define INSTRENDERER_UNICONTAINER_H
#include "Uniforms.h"
#include "RenderGraph/RenderGraph.h"

class UniBase
{
public:
    inline void Setup(std::string name,RDG::RenderGraph& renderGraph)
    {
        this->handle = renderGraph.GetResourceHandle(name);
        rg = &renderGraph;
    };
    virtual void UpdateData() = 0;
    virtual void InitData() = 0;

public:
    Handle handle;
   RDG::RenderGraph* rg;
};

template<typename T>
class UniContainer:public UniBase
{
public:
    std::function<void()> CustomUpdate;
    std::function<void()> CustomInit;
    void UpdateData() override;
    void InitData() override;
    void MemoryCopy();
    T data;
};

template<typename T>
void UniContainer<T>::InitData()
{
    if(CustomInit)
    {
        CustomInit();
        //MemoryCopy();
    }
}

template<typename T>
using UniPtr = std::shared_ptr<UniContainer<T>>;
#define INIT_UNIPTR(T) std::make_shared<UniContainer<T>>();

template<typename T>
void UniContainer<T>::UpdateData()
{
    if(CustomUpdate)
    {
        CustomUpdate();
        MemoryCopy();
    }
}

template<typename T>
void UniContainer<T>::MemoryCopy()
{
    const RDG::ResourceRef& res = rg->resourceMap[handle];
    if(!res.bufferInfo.has_value())
        std::cout<<"copy to null address\n";
    memcpy(res.bufferInfo->mappedAddress,&data,sizeof(T));
}




#endif //INSTRENDERER_UNICONTAINER_H
