#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPostGlitch.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxPostProcessing.h"


class testApp : public ofBaseApp, public ofxMidiListener {

public:
	void setup();
	void update();
	void draw();
    void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    void newMidiMessage(ofxMidiMessage& eventArgs);

	ofImage			lenna;
	bool			bDrawLenna;
	bool			bShowHelp;

	ofFbo			myFbo;
	ofEasyCam		camera;

	ofTexture		texture;

	/*Addon instance*/
	ofxPostGlitch	myGlitch;
    
    // recording
    ofxImageSequenceRecorder recorder;
    Boolean recording;
    ofImage screenCapture;
    
    // midi
    ofxMidiIn midiIn;
};
