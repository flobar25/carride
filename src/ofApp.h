#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxPostGlitch.h"
#include "Floor.h"
#include "Space.h"
#include "Ribbon.h"
#include "Dots.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    
public:
    ofApp();
    ~ofApp();
    void setup();
    void exit();
    void update();
    void draw();
    
    // recording
    void toggleRecording();
    void captureScreen();
    
    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    // cam
    void setRandomCamPosition();
    
    // visual
    void addRibbon();
    void addDots();
    
    // events
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
    ofVec3f camPosition;
    int roll;
    
    // recording
    ofxImageSequenceRecorder recorder;
    bool recording;
    ofImage screenCapture;
    
    // elements of the visual
    Floor* floor;
    Space* space;
    vector<Dots*> dots;
    vector<Ribbon*> ribbons;
    
    //shaders
    ofShader floorShader;
    ofShader spaceShader;
    ofShader dotShader;
    ofShader dummyShader;
    ofShader invertShader;
    
    // post procesing
    ofxPostGlitch postGlitch;
    ofFbo fbo;
};
