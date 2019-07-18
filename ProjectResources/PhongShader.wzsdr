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
    vec3 rotation;
    vec4 color;
    float intensity;
    float range;
};

uniform vec4 u_albedo = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 u_diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform bool u_useDiffuseMap = false;
uniform sampler2D u_diffuseMap;

uniform vec4 u_ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform int u_nLights = 0;
uniform Light u_lights[1024];

void main()
{
    vec4 diffuseColor = u_diffuseColor;
    vec3 diffuse = vec3(1.0, 1.0, 1.0);

    if (u_useDiffuseMap) {
        diffuseColor = texture(u_diffuseMap, uv);
    }

    for (int i = 0; i < u_nLights; i++) {
        vec3 lightPos = u_lights[i].position;//(camTransform * vec4(u_lights[i].position, 1.0)).xyz;
        vec3 lightDiff = lightPos - fragPos;
        float lightDist = length(lightDiff);
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightDiff);

        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * u_lights[i].color.xyz * diffuseColor.xyz;
    }

    outColor = u_albedo * ((u_ambient) + vec4(diffuse, 1.0));
}
