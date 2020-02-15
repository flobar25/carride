#include <stdio.h>
#include "ofMain.h"

class Dots {
public:
    Dots(int dotsCount, float distance, ofPoint dotsCenter, ofTexture texture);
    void draw();
    
private:
    ofMesh dotsMesh;
    ofTexture dotsTexture;
};
