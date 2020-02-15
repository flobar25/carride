#include "ofMain.h"
class Floor {
public:
    Floor(int height, int width, int trianglesSize, int trianglesSetCount, vector<ofColor_<float> >& colors, float waveSpeed);
    void toggleTriangles(int i);
    void startWave();
    void update();
    void draw();
    void setTargetWaveValue(float value);
    float getWaveValue();
    
    
private:
    ofMesh floorMesh;
    vector<ofMesh> trianglesMeshes;
    vector<bool> tranglesActivation;
    float waveValue;
    float targetWaveValue;
    float waveStartTime;
    float waveSpeed;
};

