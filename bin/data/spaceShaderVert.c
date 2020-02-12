#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float value = 0.0;
uniform float elapsedTime = 0.0;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
    float fl = floor(p);
  float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}

void main() {
    vec3 v = gl_Vertex.xyz;
    if (value > 0){
            v.z += noise(value + v.z) * 5;
            v.y += noise(value + v.y) * 5;
            v.z += noise(value + v.z) * 5;
    }
    
    vec4 posHomog = vec4(v, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}

