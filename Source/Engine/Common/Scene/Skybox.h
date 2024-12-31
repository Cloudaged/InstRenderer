
#ifndef INSTRENDERER_SKYBOX_H
#define INSTRENDERER_SKYBOX_H
#include "../../Render/Mesh.h"
#include "../../Render/Buffer/Texture.h"
#include "../../Resource/ModelLoader.h"
#include "../../Resource/ImageLoader.h"
class Skybox
{
public:
    Skybox(std::string boxPath, std::vector<std::string> paths);
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    int width = 0;
    int height = 0;
};


#endif //INSTRENDERER_SKYBOX_H
