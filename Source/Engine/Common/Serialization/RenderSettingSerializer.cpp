
#include "RenderSettingSerializer.h"
#include "../System/RenderSystem.h"
RenderSettings RenderSettingSerializer::FromJson(nlohmann::json j)
{
    RenderSettings settings;
    settings.uniform.shadowDebug.pcfSampleCount = j["ShadowSetting"]["PcfSampleCount"];
    settings.uniform.shadowDebug.blockerSearchCount = j["ShadowSetting"]["BlockerSearchCount"];
    settings.defaultLightIntensity = j["LightSetting"]["DefaultLightIntensity"];
    settings.defaultLightRange = j["LightSetting"]["DefaultLightRange"];
    return settings;
}
