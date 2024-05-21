#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 uv;
layout(Location = 2) in vec4 worldPos;
layout(location = 3) in float visibility;

layout(push_constant) uniform constants {
  uint globalDataBinding;
  uint grassBind;
  uint snowBind;
  uint rock1Bind;
  uint rock2Bind;
};

layout(binding = 0) uniform sampler2D textures[];

float contribution(float val, float minval, float maxval) {
  return clamp(((val - minval) / (maxval - minval)), 0, 1);
}

vec4 getTextColor() {
  float H = worldPos.y;
  vec4 grass = texture(textures[grassBind], uv);
  vec4 snow = texture(textures[snowBind], uv);
  vec4 rock1 = texture(textures[rock1Bind], uv * 2);
  vec4 rock2 = texture(textures[rock2Bind], uv);

  return mix(grass,
    mix(rock2,
      mix(rock1, snow, contribution(H, 60, 70)),
      contribution(H, 35, 42)),
    contribution(H, 27, 30));

}


void main() {
  outColor = mix(vec4(0.2,0.4,0.8,1), getTextColor() * clamp(dot(normal, normalize(vec3(1, -1, 1))), 0.2, 1), visibility);
}
