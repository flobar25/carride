#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    static const int TRIANGLE_SIZE = 10;
    static const int MESH_WIDTH = 100;
    static const int MESH_HEIGHT = 100;
    
public:
    void setup();
    void exit();
    void update();
    void draw();
    
    void initMesh(int width, int height);
    
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
    ofMesh mesh;
};
