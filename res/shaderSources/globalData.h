layout(binding = 1) buffer GlobalData {
    mat4 proj;
    mat4 view;
    mat4 projView;
    vec3 camPos;
    vec4 frustum[6];
    vec4 skyColor;
    float fogDensity;
    float fogGradient;
} globalData[];

#define global (globalData[globalDataBinding])