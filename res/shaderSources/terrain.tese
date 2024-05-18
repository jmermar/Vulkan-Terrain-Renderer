#version 450

layout(quads, equal_spacing, ccw) in;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec4 worldPos;

layout(push_constant) uniform constants {
  mat4 proj;
  mat4 view;
  mat4 model;
  uint grassBind;
};

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

float getHeight(vec2 v) {
  float f = 1.0 / 128.0;
  float amplitude = 1;
  float ampAcc = 0;  
  float noiseAcc = 0;
  for(int i = 0; i < 8; i++) {
    noiseAcc += (snoise(v * f) * 0.5 + 0.5) * amplitude;
    ampAcc += amplitude;
    f *= 4;
    amplitude /= 4;
  }
  
  return noiseAcc / ampAcc;
}

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec4 p00 = gl_in[0].gl_Position;
  vec4 p01 = gl_in[1].gl_Position;
  vec4 p10 = gl_in[2].gl_Position;
  vec4 p11 = gl_in[3].gl_Position;

  vec4 p0 = (p01 - p00) * u + p00;
  vec4 p1 = (p11 - p10) * u + p10;
  worldPos = (p1 - p0) * v + p0;


  worldPos.y = getHeight(vec2(worldPos.x, worldPos.z));
	outColor = vec3(clamp(worldPos.y, 0.3, 1));
	worldPos.y *= 90;
	uv.x = worldPos.x / 8;
	uv.y = worldPos.z / 8;
	gl_Position = proj * view * worldPos;
}