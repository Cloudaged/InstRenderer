
#ifndef INSTRENDERER_RESMATERIAL_H
#define INSTRENDERER_RESMATERIAL_H
#include "vector"
#include "ResTexture.h"
#include "memory"
namespace Res
{

#define RESTAG(resName) resName + "_RES"
    class ResMaterial
    {
    public:
        void AddTexture(std::shared_ptr<ResTexture> texture);
        std::vector<std::shared_ptr<ResTexture>> textures;
    private:
    };

} // Res

#endif //INSTRENDERER_RESMATERIAL_H
