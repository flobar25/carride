#include "Space.h"

Space::Space(int height, int width, int trianglesSize, int intensityThreshold, int connectionDistance, string imagePath) {
    jitterValue = 0;
    
    ofImage spaceImage;
    spaceImage.load(imagePath);
    spaceImage.resize(width*2, height*2);
    
    spaceMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
    spaceMesh.enableColors();
    spaceMesh.enableIndices();
    
    int w = spaceImage.getWidth();
    int h = spaceImage.getHeight();
    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
            ofColor c = spaceImage.getColor(x, y);
            float intensity = c.getLightness();
            if (intensity >= intensityThreshold) {
                ofVec3f pos(x * (width*trianglesSize / spaceImage.getWidth()), y * (height*trianglesSize / spaceImage.getHeight()), 300 + ofNoise(x, y) * 50);
                spaceMesh.addVertex(pos);
                spaceMesh.addColor(c);
            }
        }
    }
    
    int numVerts = spaceMesh.getNumVertices();
    for (int a=0; a<numVerts; ++a) {
        ofVec3f verta = spaceMesh.getVertex(a);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = spaceMesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance <= connectionDistance) {
                spaceMesh.addIndex(a);
                spaceMesh.addIndex(b);
            }
        }
    }
    
}

void Space::startJitter() {
    jitterValue = 2;
    jitterStartTime = ofGetElapsedTimef() ;
}

void Space::update(){
    if (jitterValue > 0) {
        jitterValue -= ofGetElapsedTimef() - jitterStartTime;
    }
}

void Space::draw() {
    spaceMesh.draw();
}

float Space::getJitterValue() {
    return jitterValue;
}

ofMesh Space::getSpaceMesh() {
    return spaceMesh;
}
