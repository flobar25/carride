#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float value = 0.0;
uniform float cameraZ = 0;
uniform int meshHeight = 0;

void main() {
    vec3 v = gl_Vertex.xyz;
    
    if (v.z > 4.0) {
        v.z = v.z + value *10;
    }
    if (v.z < -4.0) {
        v.z = v.z - value *10;
    }
    
//    if (v.y < -cameraZ){
//        //v.y += meshHeight * (1 + (-cameraZ / meshHeight));
//        v.y += 2500;
//    }
    
    vec4 posHomog = vec4(v, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
