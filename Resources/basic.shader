#shader vertex

#version 330 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 worldPosition;
out vec4 fragColor;

void main() {
    worldPosition = projection * view * model * vertexPosition;
    fragColor = vertexColor;
    gl_Position = worldPosition;
}

#shader fragment

#version 330 core

out vec4 outColor;
in vec4 fragColor;

in vec4 worldPosition;

void main()
{
    outColor = fragColor;
}
