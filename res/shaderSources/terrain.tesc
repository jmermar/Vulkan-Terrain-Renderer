#version 450

layout (vertices = 4) out;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    gl_TessLevelOuter[0] = 128;
    gl_TessLevelOuter[1] = 128;
    gl_TessLevelOuter[2] = 128;
    gl_TessLevelOuter[3] = 128;

    gl_TessLevelInner[0] = 128;
    gl_TessLevelInner[1] = 128;
}