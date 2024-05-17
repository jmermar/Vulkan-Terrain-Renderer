#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 uv;

layout(push_constant) uniform constants {
  mat4 proj;
  mat4 view;
  mat4 model;
  uint grassBind;
};

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
p /= 20;
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

void main() {
vec4 worldPos = model * vec4(pos, 1.0);
worldPos.y =noise(vec2(worldPos.x, worldPos.z));
outColor = vec3(worldPos.y);
worldPos.y *= 30;
uv.x = worldPos.x;
uv.y = worldPos.z;
gl_Position = proj * view * worldPos;
}
