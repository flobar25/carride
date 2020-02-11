#include "ofMain.h"
//#include "testApp.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(2048, 1024,OF_WINDOW);			// <-------- setup the GL context
//    ofSetupOpenGL(2560, 1440,OF_WINDOW);            // <-------- setup the GL context
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
    
//    ofGLFWWindowSettings settings;
//
//    settings.setGLVersion(2, 1);
//    settings.setSize(1024, 512);
//    settings.setPosition(ofVec2f(100,0));
//    settings.resizable = true;
//    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
//
//    shared_ptr<testApp> mainApp(new testApp);
//    ofRunApp(mainWindow, mainApp);
//    ofRunMainLoop();


}
