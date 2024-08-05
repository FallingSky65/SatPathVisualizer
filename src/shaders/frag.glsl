#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};


// Input lighting values
uniform vec3 viewPos;

// https://github.com/minus34/cesium1/blob/master/Cesium/Shaders/Builtin/Functions/saturation.glsl
vec3 czm_saturation(vec3 rgb, float adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

vec4 fade(vec4 rgba, float adjustment) {
    return vec4(rgba.rgb, rgba.a*adjustment);
}

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    //finalColor = texelColor;
   
    vec3 viewDir = normalize(vec3(0.0) - viewPos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 left = cross(viewDir, up);
    
    vec3 lightPos = viewDir+up+left;

    vec3 light = normalize(lightPos);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    float ambient = 0.1;

    float fresnel = dot(normal, viewD);
    fresnel = pow(1 - fresnel, 36);
    fresnel = clamp(fresnel, 0.0, 1.0);
    vec3 fresnelColor = fresnel*czm_saturation(vec3(3, 167, 255), 0.5);

    vec4 specular = vec4(vec3(0.6*pow(max(0.0, dot(viewD, reflect(light, normal))), 15.0)), 1.0);

    float brightness = max(dot(normal, -light), 0.0) + ambient;
    finalColor = texelColor*vec4((brightness - ambient)*(colDiffuse.xyz + specular.xyz) + ambient, 1.0);
    finalColor += vec4(pow(brightness, 3.0)*0.2*fresnelColor, 1.0);

    // Gamma correction
    finalColor = clamp(pow(finalColor, vec4(1.0/1.4)), 0.0, 1.0);
    finalColor = vec4(czm_saturation(finalColor.rgb, 1.5), 1.0);

    float featureOpacity = 0.7;
    vec4 featureColor1 = texture(texture1, fragTexCoord);
    vec4 featureColor2 = texture(texture2, fragTexCoord);
    finalColor = mix(finalColor, mix(mix(finalColor, featureColor1+featureColor2, featureColor1.a+featureColor2.a), mix(featureColor1, featureColor2, 0.5), featureColor1.a*featureColor2.a), featureOpacity);
}
