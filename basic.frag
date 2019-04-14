#version 410 core

out vec4 color;

in vec4 pos;

void main() {
    color = vec4(pos.x / 16.0, pos.y / 9.0, pos.z / 12.5, 1.0);
}
