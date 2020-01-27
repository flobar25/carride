#include "ofMain.h"
class Floor {
public:
    Floor(int height, int width, int trianglesSize, int trianglesSetCount, vector<ofColor_<float> >& colors);
    void toggleTriangles(int i);
    void startWave();
    void update();
    void draw();
    float getWaveValue();
    
    
private:
    ofMesh floorMesh;
    vector<ofMesh> trianglesMeshes;
    vector<bool> tranglesActivation;
    float waveValue;
    float waveStartTime;
};

