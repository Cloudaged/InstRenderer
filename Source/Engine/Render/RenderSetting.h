
#ifndef INSTRENDERER_RENDERSETTING_H
#define INSTRENDERER_RENDERSETTING_H
struct ShadowSetting
{
    int pcfSampleCount = 32;
    int blockerSearchCount = 32;
};

struct RenderSettingData
{
    ShadowSetting shadowDebug{};
};

#endif //INSTRENDERER_RENDERSETTING_H
