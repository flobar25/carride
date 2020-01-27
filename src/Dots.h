#include <stdio.h>
#include "ofMain.h"

class Dots {
public:
    Dots(int dotsCount, float distance, ofPoint dotsCenter, string texturePath);
    
private:
    ofMesh dotsMesh;
    ofTexture dotsTexture;
};
