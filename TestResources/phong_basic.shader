#lightmode phong

#shader vertex

#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 u_camTransform;
uniform mat4 u_worldTransform;

out vec3 fragPos;
out vec2 uv;
out mat4 camTransform;
out vec3 normal;

void main() {
    gl_Position = u_camTransform * u_worldTransform * vec4(vertexPosition, 1.0);

    fragPos = (u_camTransform * vec4(vertexPosition, 1.0)).xyz;
    uv = vertexUv;
    camTransform = u_camTransform;
    normal = mat3(transpose(inverse(u_worldTransform))) * vertexNormal;
	fragPos = (u_worldTransform * vec4(vertexPosition, 1.0)).xyz;
}




#shader fragment

#version 330 core

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

out vec4 outColor;

in vec3 fragPos;
in mat4 camTransform;
in vec2 uv;
in vec3 normal;

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
    float range;
    float cutOff;
    float smoothness;
};

uniform vec4 u_albedo = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 u_diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform bool u_useDiffuseMap = false;
uniform sampler2D u_diffuseMap;
uniform vec4 u_specularColor = vec4(1.0, 1.0, 1.0, 0.5);
uniform bool u_useSpecularMap = false;
uniform sampler2D u_specularMap;

uniform vec3 u_viewPos = vec3(-10.0, 0.0, 0.0);
uniform vec4 u_ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform int u_nLights = 0;
uniform Light u_lights[64];

void CalculateDirectionalLight(vec3 direction, vec4 color, float intensity, float metallic, inout vec3 diffuse, inout vec3 specular) {

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-direction);

    float diff = max(dot(norm, lightDir), 0.0);
    diffuse += diff * color.xyz  * intensity;

    // specular
    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), metallic);
    specular += spec * color.xyz * intensity;
}

void CalculatePointLight(vec3 position, vec4 color, float intensity, float metallic, float range, inout vec3 diffuse, inout vec3 specular) {
    vec3 lightPos = position;
    vec3 lightDiff = lightPos - fragPos;
    float lightDist = length(lightDiff);

    float distanceIntensity = (range - lightDist) / range;
    if (distanceIntensity > 1.0) distanceIntensity = 1.0;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightDiff);

    float diff = max(dot(norm, lightDir), 0.0);
    diffuse += diff * color.xyz  * intensity * distanceIntensity;

    // specular
    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), metallic);
    specular += spec * color.xyz * intensity * distanceIntensity;
}

void CalculateSpotLight(vec3 position, vec3 direction, vec4 color, float intensity, float metallic, float range, float cutOff, float smoothness, inout vec3 diffuse, inout vec3 specular) {

    vec3 lightPos = position;
    vec3 lightDiff = lightPos - fragPos;
    vec3 lightDir = normalize(lightDiff);

    smoothness = clamp(smoothness, 0.01, 1.0);

    float theta = dot(lightDir, normalize(-direction));
    float epsilon = (1.0 - smoothness) * cutOff;
    intensity *= clamp((theta - cutOff) / epsilon, 0.0, 1.0);

    if(theta > cutOff) {
        CalculatePointLight(position, color, intensity, metallic, range, diffuse, specular);
    }
}

void main()
{
    float metallic = u_specularColor.w * 256;
    if (metallic < 0.137) metallic = 0.137;
    vec4 diffuseColor = u_diffuseColor;
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec4 specularColor = u_specularColor;
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if (u_useDiffuseMap) {
        diffuseColor = texture(u_diffuseMap, uv);
    }
    if (u_useSpecularMap) {
        specularColor = texture(u_specularMap, uv);
    }

    for (int i = 0; i < u_nLights; i++) {

        switch (u_lights[i].type) {
            case LIGHT_TYPE_DIRECTIONAL:
                CalculateDirectionalLight(u_lights[i].direction, u_lights[i].color, u_lights[i].intensity, metallic, diffuse, specular);
                break;
            case LIGHT_TYPE_POINT:
                CalculatePointLight(u_lights[i].position, u_lights[i].color, u_lights[i].intensity, metallic, u_lights[i].range, diffuse, specular);
                break;
            case LIGHT_TYPE_SPOT:
                CalculateSpotLight(u_lights[i].position, u_lights[i].direction, u_lights[i].color, u_lights[i].intensity, metallic, u_lights[i].range, u_lights[i].cutOff, u_lights[i].smoothness, diffuse, specular);
                break;
        }

    }

    outColor = u_albedo * (u_ambient + vec4(diffuse + specular * specularColor.xyz, 1.0)) * diffuseColor;
}
