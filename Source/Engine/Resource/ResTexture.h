
#ifndef INSTRENDERER_RESTEXTURE_H
#define INSTRENDERER_RESTEXTURE_H

namespace Res
{

    class ResTexture
    {
    public:
        ResTexture(int width,int height,int channel,void* data);
        int width,height,channel,size;
        void* data;
    };

} // Res

#endif //INSTRENDERER_RESTEXTURE_H
