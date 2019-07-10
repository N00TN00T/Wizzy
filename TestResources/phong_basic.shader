#lightmode phong

#shader vertex

#version 330 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 camTransform;
uniform mat4 worldTransform;

out vec4 worldPosition;
out vec2 uv;

void main() {
    worldPosition = camTransform * worldTransform * vertexPosition;
    gl_Position = worldPosition;

    uv = vertexUv;
}




#shader fragment

#version 330 core

out vec4 outColor;

in vec4 worldPosition;
in vec2 uv;

uniform vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform bool useDiffuseMap = false;
uniform sampler2D diffuseMap;

void main()
{
    vec4 textureColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (useDiffuseMap) {
        textureColor = texture(diffuseMap, uv);
    }
    outColor = textureColor * diffuseColor * albedo;
}
