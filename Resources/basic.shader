#shader vertex

#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec3 worldPosition;
out vec4 fragColor;

void main() {
    worldPosition = vertexPosition;
    fragColor = vertexColor;
    gl_Position = vec4(worldPosition, 1.0);
}

#shader fragment

#version 330 core

out vec4 outColor;
in vec4 fragColor;

in vec3 worldPosition;

void main()
{
    outColor = fragColor;
}
