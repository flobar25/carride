#include "Line.h"

Line::Line(ofPoint& startingPoint, ofVec3f& startingDirection, float speed, int xb, int zb, int mp){
    lineMesh.addVertex(startingPoint);
    direction = startingDirection;
    lastPoint = startingPoint;
    lineSpeed = speed;
    xBoundary = xb;
    zBoundary = zb;
    maxPointsCount = mp;
}

void Line::changeDirection(){
    float x = rand() % 3 - 1;
    float y = 1;
    float z = rand() % 3 - 1;
    direction = ofVec3f(x, y, z);
    //lineDirection.normalize();
    direction *= lineSpeed;
    direction.y = lineSpeed;
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(direction));
}

void Line::update() {
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
    lineMesh.addVertex(newPoint);
    if (lineMesh.getVertices().size() > maxPointsCount) {
        lineMesh.getVertices().erase(lineMesh.getVertices().begin());
    }
}
