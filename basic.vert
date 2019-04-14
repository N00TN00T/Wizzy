#version 410 core

layout (location = 0) in vec4 position;

uniform mat4 projection;
uniform mat4 view;

out vec4 pos;

void main() {
    pos = projection * view * position;
    gl_Position = pos;
}
