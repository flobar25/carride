#include <stdio.h>
#include "ofMain.h"

class Dots {
public:
    Dots(int dotsCount, float distance, ofPoint dotsCenter, string texturePath);
    void draw();
    
private:
    ofMesh dotsMesh;
    ofTexture dotsTexture;
};
