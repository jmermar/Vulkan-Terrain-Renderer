#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv;
layout(Location = 2) in vec4 worldPos;

layout(push_constant) uniform constants {
  mat4 proj;
  mat4 view;
  mat4 model;
  uint grassBind;
  uint snowBind;
  uint rock1Bind;
  uint rock2Bind;
};

layout(binding = 0) uniform sampler2D textures[];


float contribution(float val, float minval, float maxval) {
  return clamp((val - minval) / maxval, 0, 1);
}

vec4 getTextColor() {
  float H = worldPos.y;
  vec4 grass = texture(textures[grassBind], uv);
  vec4 snow = texture(textures[snowBind], uv);
  vec4 rock1 = texture(textures[rock1Bind], uv);
  vec4 rock2 = texture(textures[rock2Bind], uv);

  return mix(grass,
    mix(rock2,
      mix(rock1, snow, contribution(H, 59.5, 60)),
      contribution(H, 25.5, 30)),
    contribution(H, 14, 14.5));

}


void main() {
  outColor = getTextColor();
}
