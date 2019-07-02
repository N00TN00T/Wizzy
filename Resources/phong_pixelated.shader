#shader vertex

#version 330  core

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
uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform float pixels = 2048.0;

void main()
{
    if (hasTexture) {
        float dx = (16.0 * (1.0 / pixels));
        float dy = (10.0 * (1.0 / pixels));
        vec2 coord = vec2(dx * floor(uv.x / dx) + dx / 2.0,
                          dy * floor(uv.y / dy) + dy / 2.0);
        outColor = texture(diffuseTexture, coord);
    } else {
        outColor = diffuseColor * albedo;
    }
}
