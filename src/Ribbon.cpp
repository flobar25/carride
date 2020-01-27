#include "Ribbon.h"

Ribbon::Ribbon(ofPoint startingPoint, ofVec3f startingDirection, float speed, int xb, int zb, int mp){
    ribbonMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
    ribbonMesh.addVertex(startingPoint);
    direction = startingDirection;
    lastPoint = startingPoint;
    ribbonSpeed = speed;
    xBoundary = xb;
    zBoundary = zb;
    maxPointsCount = mp;
}

void Ribbon::changeDirection(){
    float x = rand() % 3 - 1;
    float y = 1;
    float z = rand() % 3 - 1;
    direction = ofVec3f(x, y, z);
    //lineDirection.normalize();
    direction *= ribbonSpeed;
    direction.y = ribbonSpeed;
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(direction));
}

void Ribbon::update() {
    if (lastPoint.x + direction.x < 0 || lastPoint.x + direction.x > xBoundary) {
        direction.x = direction.x;
    }
    if (lastPoint.z + direction.z > zBoundary || lastPoint.z + direction.z < -zBoundary) {
        direction.z = -direction.z;
    }
    
    int x = lastPoint.x + direction.x;
    int y = lastPoint.y + direction.y;
    int z = lastPoint.z + direction.z;
    
    ofPoint newPoint = ofPoint(x, y, z);
    lastPoint = newPoint;
    ribbonMesh.addVertex(newPoint);
    if (ribbonMesh.getVertices().size() > maxPointsCount) {
        ribbonMesh.getVertices().erase(ribbonMesh.getVertices().begin());
    }
}

void Ribbon::draw() {
    // draw lines
    ofSetLineWidth(5);
    ofSetColor(ofColor::red);
    ribbonMesh.draw();
    ofSetLineWidth(1);
}
