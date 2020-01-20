#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float value = 0.0;
uniform float elapsedTime = 0.0;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec3 v = gl_Vertex.xyz;
    if (value > 0){
            v.z = v.z + sin(value + 10 * rand(v.xy)) * 2;
            v.y = v.y + sin(value * 17 + 10 * rand(v.xy)) * 2;
            v.x = v.x + sin(value * 11 + 10 * rand(v.xy)) * 2;
    }
    
    vec4 posHomog = vec4(v, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}

