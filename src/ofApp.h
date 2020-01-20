#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    static const int TRIANGLE_SIZE = 10;
    static const int MESH_WIDTH = 100;
    static const int MESH_HEIGHT = 250;
    
public:
    void setup();
    void exit();
    void update();
    void draw();
    
    void initMesh(int width, int height);
    void initSphere();
    void initSpaceMesh();
    
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
    
private:
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
    ofMesh trianglesMesh;
    ofMesh sphere;
    ofMesh spaceMesh;
    ofImage spaceImage;
    
    //shaders
    ofShader floorShader;
    ofShader spaceShader;
    
    ofxPostProcessing post;
    ofFbo fbo;

};
