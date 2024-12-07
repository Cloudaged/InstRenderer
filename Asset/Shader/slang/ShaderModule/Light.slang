module Light;
import Types;

enum LightType
{
    Directional = 0,
    Point = 1,
    Spot = 2
}

public struct LightProperty
{
    public float3 lightDir;
    public float intensity;
}

public LightProperty GetLightProperty(Light light)
{
    LightProperty lightProperty;
    if (light.type == LightType::Directional)
    {
        lightProperty.lightDir = -normalize(light.toward.xyz);
        lightProperty.intensity = light.intensity;
    } else if (light.type == LightType::Point)
    {
        lightProperty.lightDir = normalize(light.position.xyz - light.toward.xyz);
        lightProperty.intensity = light.intensity;
    } else if (light.type == LightType::Spot)
    {
        lightProperty.lightDir = normalize(light.position.xyz - light.toward.xyz);
        lightProperty.intensity = light.intensity;
    }
    return lightProperty;
}