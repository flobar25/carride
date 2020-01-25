#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxPostGlitch.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    
public:
    void setup();
    void exit();
    void update();
    void draw();
    
    void initMeshes(int width, int height);

    // recording
    void toggleRecording();
    void captureScreen();
    
    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //events
    void startWave();
    void startJitter();
    void triggerTriangles(int i);
    
private:
    // init different meshes
    void initSpaceMesh();
    void initFloorMesh(int height, int width);
    void initDotsMesh();
    void initSphere();
    
    // midi
    ofxMidiIn midiIn;
    
    // camera
    ofEasyCam cam;
    
    // recording
    ofxImageSequenceRecorder recorder;
    Boolean recording;
    ofImage screenCapture;
    
    // elements of the visual
    ofMesh floorMesh;
    vector<ofMesh> trianglesMeshes;
    vector<bool> trianglesMeshesActivation;
    ofMesh sphere;
    ofMesh spaceMesh;
    ofImage spaceImage;
    ofMesh dotsMesh;
    ofTexture dotsTexture;
    
    //shaders
    ofShader floorShader;
    ofShader spaceShader;
    ofShader dotShader;
    ofShader dummyShader;
    
    // post procesing
    ofxPostGlitch postGlitch;
    ofFbo fbo;
    
};
