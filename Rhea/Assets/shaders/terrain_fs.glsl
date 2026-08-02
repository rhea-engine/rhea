#version 330 core

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
    vec4 DirectionType;   // xyz = direction, w = type
    vec4 Position;
    vec4 Color;
    vec4 Diffuse;
    vec4 Specular;
    vec4 SpotParams;      // x = SpotAngle, y = Constant, z = Linear, w = Quadratic
};

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPosition;
};

layout(std140) uniform cbLight
{
    Light lights[8];
    vec4 ambient;
    float lightCount;
    vec3 _cbLightPad0;
};

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
uniform sampler2D Texture4;

in vec3 v_normal;
in vec2 v_tex;
in vec4 v_positionWS;

out vec4 FragColor;

vec4 DoSpecularPhong(Light light, vec3 V, vec3 L, vec3 N)
{
    vec3 R = normalize(reflect(-L, N));
    float RdotV = max(0.0, dot(R, V));
    return light.Color * pow(RdotV, 4.0);
}

vec4 DoDiffuse(Light light, vec3 L, vec3 N)
{
    float NdotL = max(0.0, dot(N, L));
    return light.Color * NdotL;
}

struct LightingResult
{
    vec4 Diffuse;
    vec4 Specular;
};

LightingResult DoDirectionalLight(Light light, vec3 View, vec4 P, vec3 N)
{
    LightingResult result;
    vec3 L = -light.DirectionType.xyz;
    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecularPhong(light, View, L, N);
    return result;
}

float DoAttenuation(Light light, float d)
{
    float att = light.SpotParams.y
              + light.SpotParams.z * d
              + light.SpotParams.w * d * d;

    if (att == 0.0)
        return 0.0;

    return 1.0 / att;
}

LightingResult DoPointLight(Light light, vec3 V, vec4 P, vec3 N)
{
    LightingResult result;

    vec3 L = (light.Position - P).xyz;
    float distanceValue = length(L);
    L = L / distanceValue;

    float attenuation = DoAttenuation(light, distanceValue);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation;

    return result;
}

float DoSpotCone(Light light, vec3 L)
{
    float minCos = cos(light.SpotParams.x);
    float maxCos = (minCos + 1.0) / 2.0;
    float cosAngle = dot(light.DirectionType.xyz, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, vec3 V, vec4 P, vec3 N)
{
    LightingResult result;

    vec3 L = (light.Position - P).xyz;
    float distanceValue = length(L);
    L = L / distanceValue;

    float attenuation = DoAttenuation(light, distanceValue);
    float spotIntensity = DoSpotCone(light, L);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation * spotIntensity;

    return result;
}

LightingResult ComputeLighting(vec4 P, vec3 N)
{
    LightingResult result;
    result.Diffuse = vec4(0.0);
    result.Specular = vec4(0.0);

    vec3 View = normalize((cameraPosition - P).xyz);

    for (int i = 0; i < int(lightCount); ++i)
    {
        LightingResult singleResult;
        singleResult.Diffuse = vec4(0.0);
        singleResult.Specular = vec4(0.0);

        int type = int(lights[i].DirectionType.w);

        if (type == DIRECTIONAL_LIGHT)
            singleResult = DoDirectionalLight(lights[i], View, P, N);
        else if (type == POINT_LIGHT)
            singleResult = DoPointLight(lights[i], View, P, N);
        else if (type == SPOT_LIGHT)
            singleResult = DoSpotLight(lights[i], View, P, N);

        result.Diffuse += singleResult.Diffuse;
        result.Specular += singleResult.Specular;
    }

    result.Diffuse = clamp(result.Diffuse, 0.0, 1.0);
    result.Specular = clamp(result.Specular, 0.0, 1.0);

    return result;
}

void main()
{
    vec4 blendColor = texture(Texture0, v_tex);

    float bgAmount = 1.0 - (blendColor.r + blendColor.g + blendColor.b);
    float tileSize = 1.0;
    vec2 tiledCoord = vec2(v_positionWS.x, -v_positionWS.z) / tileSize;

    vec4 tex1col = texture(Texture1, tiledCoord) * bgAmount;
    vec4 tex2col = texture(Texture2, tiledCoord) * blendColor.r;
    vec4 tex3col = texture(Texture3, tiledCoord) * blendColor.g;
    vec4 tex4col = texture(Texture4, tiledCoord) * blendColor.b;

    vec4 totalColor = tex1col + tex2col + tex3col + tex4col;

    LightingResult lit = ComputeLighting(v_positionWS, normalize(v_normal));

    vec3 finalColor = (ambient.rgb + lit.Diffuse.rgb + lit.Specular.rgb) * totalColor.rgb;

    FragColor = vec4(finalColor, totalColor.a);
}