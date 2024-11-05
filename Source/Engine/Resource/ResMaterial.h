
#ifndef INSTRENDERER_RESMATERIAL_H
#define INSTRENDERER_RESMATERIAL_H
#include "vector"
#include "ResTexture.h"
namespace Res
{

    class ResMaterial
    {
    public:
        void AddTexture(ResTexture* texture);
        std::vector<ResTexture*> textures;
    private:
    };

} // Res

#endif //INSTRENDERER_RESMATERIAL_H
