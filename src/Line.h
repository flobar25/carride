#include <stdio.h>
#include "ofMain.h"

class Line{
public:
    Line(ofPoint& startingPoint, ofVec3f& startingDirection, float speed, int xBoundary, int zBoundary, int maxPointsCount);
    void changeDirection();
    void update();
    
private:
    ofMesh lineMesh;
    ofPoint lastPoint;
    ofVec3f direction;
    float lineSpeed;
    int xBoundary;
    int zBoundary;
    int maxPointsCount;
};
