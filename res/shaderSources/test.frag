#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv;

layout(push_constant) uniform constants {
  mat4 proj;
  mat4 view;
  mat4 model;
  uint grassBind;
};

layout(binding = 0) uniform sampler2D textures[];
void main() {
outColor = vec4(texture(textures[grassBind], uv).xyz * color, 1);
}
