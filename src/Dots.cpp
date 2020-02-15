#include "Dots.h"

Dots::Dots(int dotsCount, float distance, ofPoint dotsCenter, ofTexture texture){
    dotsTexture = ofTexture();
    dotsMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
    for (int i = 0; i < dotsCount; i++){
        dotsMesh.addVertex(ofPoint(dotsCenter.x + ofRandom(distance), dotsCenter.y + ofRandom(distance), dotsCenter.z + ofRandom(distance)));
    }
    
    dotsTexture = texture;
}

void Dots::draw() {
    dotsTexture.bind();
    dotsMesh.draw();
    dotsTexture.unbind();
}
