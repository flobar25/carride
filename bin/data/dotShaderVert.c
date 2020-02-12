#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float ellapsedTime = 0.0;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
    float fl = floor(p);
  float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}

void main() {
    
    vec3 v = gl_Vertex.xyz;
    
    v.x += noise(v.x + ellapsedTime)*10;
    v.y += noise(v.y + ellapsedTime)*10;
    v.z += noise(v.z + ellapsedTime)*10;
    
    
    vec4 posHomog = vec4(v, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    gl_TexCoord[0] = gl_MultiTexCoord0;

	//float size    = gl_Normal.x;
    gl_PointSize  = 15;
    gl_FrontColor = gl_Color;

}


