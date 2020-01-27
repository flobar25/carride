#include <stdio.h>
#include "ofMain.h"

class Ribbon{
public:
    Ribbon(ofPoint startingPoint, ofVec3f startingDirection, float speed, int xb, int zb, int mp);
    void changeDirection();
    void update();
    void draw();
    
private:
    ofMesh ribbonMesh;
    ofPoint lastPoint;
    ofVec3f direction;
    float ribbonSpeed;
    int xBoundary;
    int zBoundary;
    int maxPointsCount;
};
