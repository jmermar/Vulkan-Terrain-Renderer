#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 normal;
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

vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


float contribution(float val, float minval, float maxval) {
  return clamp(((val - minval) / (maxval - minval)) + (snoise(uv) * 0.25), 0, 1);
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
  outColor = getTextColor() * clamp(dot(normal, normalize(vec3(1, -1, 1))), 0.2, 1);
}
