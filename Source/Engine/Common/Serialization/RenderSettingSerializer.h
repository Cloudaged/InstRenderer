
#ifndef INSTRENDERER_RENDERSETTINGSERIALIZER_H
#define INSTRENDERER_RENDERSETTINGSERIALIZER_H
#include "json.hpp"
#include "../../Render/Uniforms.h"
#include "Serializer.h"
struct RenderSettings;
class RenderSettingSerializer : public Serializer
{
public:
    nlohmann::json ToJson(const RenderSettingUniform& settings);
    RenderSettings FromJson(nlohmann::json j);

};


#endif //INSTRENDERER_RENDERSETTINGSERIALIZER_H
