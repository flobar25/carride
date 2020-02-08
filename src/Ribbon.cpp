#include "Ribbon.h"

Ribbon::Ribbon(ofPoint startingPoint, ofVec3f startingDirection, float speed, int xb, int yb, int zb, int mp){
    ribbonMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
    ribbonMesh.addVertex(startingPoint);
    direction = startingDirection;
    lastPoint = startingPoint;
    ribbonSpeed = speed;
    xBoundary = xb;
    yBoundary = yb;
    zBoundary = zb;
    maxPointsCount = mp;
    ribbonMesh.addVertex(lastPoint);
}

void Ribbon::changeDirection(float x, float y, float z) {
    direction = ofVec3f(x, y, z);
    //lineDirection.normalize();
    
    ofPoint newPoint = ofPoint(lastPoint.x, lastPoint.y, lastPoint.z);
    lastPoint = newPoint;
    ribbonMesh.addVertex(newPoint);
    
    if (ribbonMesh.getVertices().size() > maxPointsCount) {
        ribbonMesh.getVertices().erase(ribbonMesh.getVertices().begin());
    }
}

void Ribbon::changeDirection(){
    float x = (rand() % 3 - 1) * ribbonSpeed;
    float y = ribbonSpeed;
    float z = (rand() % 3 - 1) * ribbonSpeed;
    changeDirection(x, y, z);
}

void Ribbon::update() {
    if (lastPoint.x + direction.x < 0 || lastPoint.x + direction.x > xBoundary) {
        changeDirection(-direction.x, direction.y, direction.z);
    }
    if (lastPoint.z + direction.z > zBoundary || lastPoint.z + direction.z < -zBoundary) {
        changeDirection(direction.x, direction.y, -direction.z);
    }
    if (lastPoint.y + direction.y < 0 || lastPoint.y + direction.y > yBoundary) {
        changeDirection(direction.x, -direction.y, direction.z);
    }
    
    lastPoint.x = lastPoint.x + direction.x;
    lastPoint.y = lastPoint.y + direction.y;
    lastPoint.z = lastPoint.z + direction.z;
        
    ribbonMesh.setVertex(ribbonMesh.getNumVertices() - 1, lastPoint);
}

void Ribbon::draw() {
    // draw lines
    ofEnableAlphaBlending();
    ofSetLineWidth(4);
    ofSetColor(ofColor(210, 255, 252, 255));
    ribbonMesh.draw();
    ofSetLineWidth(1);
    ofDisableAlphaBlending();
}
