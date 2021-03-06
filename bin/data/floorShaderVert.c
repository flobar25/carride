#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float value = 0.0;

void main() {
    vec3 v = gl_Vertex.xyz;
    
    if (v.z > 4.0) {
        v.z = v.z + value *10;
    }
    if (v.z < -4.0) {
        v.z = v.z - value *10;
    }
    
    
    vec4 posHomog = vec4(v, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
