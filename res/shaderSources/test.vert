#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform constants {
  mat4 proj;
  mat4 view;
  mat4 model;
};

void main() {
outColor = color;
gl_Position = proj * view * model * vec4(pos, 1.0);
}
