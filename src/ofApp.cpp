#include "ofApp.h"

static const int TRIANGLE_SIZE = 10;
static const int MESH_WIDTH = 100;
static const int MESH_HEIGHT = 250;


ofApp::ofApp() {
    space = new Space(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 130.0, 100, "space3.jpg");
    floor = new Floor(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 5, space->getSpaceMesh().getColors());
    dots = new Dots(50, 200, ofPoint(500, 500, 500), "dot.png");
    ribbons.push_back(new Ribbon(ofPoint(200, 0, 200), ofVec3f(0, 5, 0), 5.0, MESH_WIDTH * TRIANGLE_SIZE, 200, 500));
    ribbons.push_back(new Ribbon(ofPoint(400, 0, 100), ofVec3f(0, 5, 0), 5.0, MESH_WIDTH * TRIANGLE_SIZE, 200, 500));
    ribbons.push_back(new Ribbon(ofPoint(300, 0, 150), ofVec3f(0, 5, 0), 5.0, MESH_WIDTH * TRIANGLE_SIZE, 200, 500));
    
    postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, false);
    postGlitch.setFx(OFXPOSTGLITCH_GLOW, false);
    postGlitch.setFx(OFXPOSTGLITCH_SHAKER, false);
    postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, false);
    postGlitch.setFx(OFXPOSTGLITCH_TWIST, false);
    postGlitch.setFx(OFXPOSTGLITCH_OUTLINE, false);
    postGlitch.setFx(OFXPOSTGLITCH_NOISE, false);
    postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, false);
    postGlitch.setFx(OFXPOSTGLITCH_SWELL, false);
    postGlitch.setFx(OFXPOSTGLITCH_INVERT, false);
    recording = false;
}

ofApp::~ofApp() {
    delete space;
    delete floor;
    delete dots;
    for (auto it = ribbons.begin(); it < ribbons.end(); it++) {
        delete *it;
    }
}

void ofApp::setup(){
    //ofsetup
    ofSetFrameRate(30);
    ofEnableDepthTest();
    
    //recording setup
    recorder.setPrefix(ofToDataPath("recording/frame_"));
    recorder.setFormat("bmp");
    recorder.startThread();
    
    // midi setup
    midiIn.openPort(0);
    midiIn.addListener(this);
    
    // shaders
    floorShader.load("floorShaderVert.c", "floorShaderFrag.c");
    spaceShader.load("spaceShaderVert.c", "spaceShaderFrag.c");
    dummyShader.load("dummyShaderVert.c", "dummyShaderFrag.c");
    dotShader.load("dotShaderVert.c", "dotShaderFrag.c");
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 0);
    postGlitch.setup(&fbo);
}

void ofApp::exit(){
    recorder.waitForThread();
    recorder.stopThread();
}

void ofApp::update(){
    // show the framerate on window title
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    //cam.enableInertia();
    cam.move(0, 0, -1 * (ofGetElapsedTimef()/3.0f));
    //cam.rotateDeg(ofNoise(ofGetElapsedTimef()) - 0.5, 0, 0, 10);
    
    for (vector<Ribbon*>::iterator it = ribbons.begin(); it < ribbons.end(); it++) {
        (*it)->update();
    }
    floor->update();
    space->update();
}

void ofApp::draw(){
    ofEnableDepthTest();
    
    // begin
    fbo.begin();
    cam.begin();
    ofClear(0,0,0,255);
    ofSetLineWidth(1);
    // matrix stuff
    ofPushMatrix();
    ofTranslate(-(MESH_WIDTH*TRIANGLE_SIZE)/2, -ofGetHeight()/2);
    ofRotateDeg(90, -1, 0, 0);
    
    // draw floor
    floorShader.begin();
    floorShader.setUniform1f("value", floor->getWaveValue());
    floorShader.setUniform1f("cameraZ", cam.getZ());
    floorShader.setUniform1f("meshHeight", MESH_HEIGHT * TRIANGLE_SIZE);
    floor->draw();
    floorShader.end();
    
    // draw space
    spaceShader.begin();
    spaceShader.setUniform1f("value", space->getJitterValue());
    space->draw();
    spaceShader.end();
    
    // draw lines
    for (auto it = ribbons.begin(); it < ribbons.end(); it++) {
        (*it)->draw();
    }

    // draw dots
    glDepthMask(GL_FALSE);
    ofSetColor(255, 100, 90);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    dotShader.begin();
    dots->draw();
    dotShader.end();
    ofDisablePointSprites();
    ofDisableBlendMode();
    glDepthMask(GL_TRUE);
    
    // close stuff
    ofPopMatrix();
    cam.end();
    fbo.end();
    
    // post processing and draw fbo
    postGlitch.generateFx();
    ofSetColor(255);
    fbo.draw(0,0);
    
    // capture the image if recording is started
    // this can slow down the rendering by a lot, so be aware of the framerate...
    // if frames are skipped this is going to mess up the rendering when imported as a video
    if (recording){
        screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        recorder.addFrame(screenCapture);
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
        case 'r':
            toggleRecording();
            break;
        case 'p':
            captureScreen();
            break;
        case 'w':
            floor->startWave();
            break;
        case 'j':
            space->startJitter();
            break;
        case 'a':
            floor->toggleTriangles(0);
            break;
        case 's':
            floor->toggleTriangles(1);
            break;
        case 'd':
            floor->toggleTriangles(2);
            break;
        case 'f':
            floor->toggleTriangles(3);
            break;
        case 'g':
            floor->toggleTriangles(4);
            break;
        case 'z':
            for (auto it = ribbons.begin(); it < ribbons.end(); it++) {
                (*it)->changeDirection();
            }
            break;
        default:
            break;
    }
    
    if (key == '1') postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , true);
    if (key == '2') postGlitch.setFx(OFXPOSTGLITCH_GLOW            , true);
    if (key == '3') postGlitch.setFx(OFXPOSTGLITCH_SHAKER            , true);
    if (key == '4') postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , true);
    if (key == '5') postGlitch.setFx(OFXPOSTGLITCH_TWIST            , true);
    if (key == '6') postGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , true);
    if (key == '7') postGlitch.setFx(OFXPOSTGLITCH_NOISE            , true);
    if (key == '8') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , true);
    if (key == '9') postGlitch.setFx(OFXPOSTGLITCH_SWELL            , true);
    if (key == '0') postGlitch.setFx(OFXPOSTGLITCH_INVERT            , true);
    
}

void ofApp::newMidiMessage(ofxMidiMessage& eventArgs){
    
}

void ofApp::captureScreen(){
    screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    screenCapture.save("captures/capture.bmp");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == '1') postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , false);
    if (key == '2') postGlitch.setFx(OFXPOSTGLITCH_GLOW            , false);
    if (key == '3') postGlitch.setFx(OFXPOSTGLITCH_SHAKER            , false);
    if (key == '4') postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , false);
    if (key == '5') postGlitch.setFx(OFXPOSTGLITCH_TWIST            , false);
    if (key == '6') postGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , false);
    if (key == '7') postGlitch.setFx(OFXPOSTGLITCH_NOISE            , false);
    if (key == '8') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , false);
    if (key == '9') postGlitch.setFx(OFXPOSTGLITCH_SWELL            , false);
    if (key == '0') postGlitch.setFx(OFXPOSTGLITCH_INVERT            , false);
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

void ofApp::toggleRecording(){
    recording = !recording;
}
