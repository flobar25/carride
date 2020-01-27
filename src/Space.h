#include "ofMain.h"
#include <stdio.h>


class Space {
    
public:
    Space(int height, int width, int trianglesSize, int intensityThreshold, int connectionDistance, string imagePath);
    void startJitter();
    float getJitterValue();
    void update();
    void draw();
    ofMesh getSpaceMesh();
    
private:
    ofMesh spaceMesh;
    float jitterValue;
    float jitterStartTime;
    
};
