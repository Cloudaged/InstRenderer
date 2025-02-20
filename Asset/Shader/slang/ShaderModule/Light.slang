module Light;
import Types;

enum LightType
{
    Directional = 0,
    Point = 1,
    Spot = 2
}

public enum SoftShadowType
{
    PCF = 0,
    PCSS = 1
}

public struct LightProperty
{
    public SoftShadowType softShadowType;
    public float3 lightDir;
    public float intensity;
}

public LightProperty GetLightProperty(Light light,float3 fragPos)
{
    LightProperty lightProperty;
    if (light.type == LightType::Directional)
    {
        lightProperty.softShadowType = SoftShadowType::PCF;
        lightProperty.lightDir = -normalize(light.toward.xyz);
        lightProperty.intensity = light.intensity;
    } else if (light.type == LightType::Point)
    {
        lightProperty.softShadowType = SoftShadowType::PCSS;
        lightProperty.lightDir = normalize(fragPos - light.position.xyz);
        lightProperty.intensity = light.intensity;
    } else if (light.type == LightType::Spot)
    {
        lightProperty.softShadowType = SoftShadowType::PCSS;
        lightProperty.lightDir = -normalize(fragPos - light.position.xyz);
        float3 l = -lightProperty.lightDir;
        float3 toward = normalize(light.toward.xyz);
        float thetaCos = dot(l, toward);
        if (cos(light.range) < thetaCos)
        {
            float soft = 1.0 - (1.0 - thetaCos) / (1.0-cos(light.range));
            lightProperty.intensity = light.intensity*soft;
        } else
        {
            lightProperty.intensity = 0;
        }
    }
    return lightProperty;
}