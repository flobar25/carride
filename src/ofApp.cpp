#include "ofApp.h"

static const int TRIANGLE_SIZE = 10;
static const int MESH_WIDTH = 250;
static const int MESH_HEIGHT = 250;
static const int MESHED_WIDTH_SIZE = MESH_WIDTH * TRIANGLE_SIZE;
static const int MESHED_HEIGHT_SIZE = MESH_HEIGHT * TRIANGLE_SIZE;
static const int CAM_MAX_X = 1000;
static const int CAM_MAX_Y = 1000;
static const int CAM_MAX_Z = 1000;
static const float CAM_NOISE_AMOUNT = 1.0;
static const int CAM_SPEED = 10;
static const float LOOK_AT_SCALE = 0.5;
static const int LOOK_AT_SPEED = 3;
static const float ROLL_SPEED = 0.02;
static const int MAX_DOTS = 50;
static const int MAX_RIBBONS = 50;
static const int TIME_TO_BUILD_FLOOR = 10000;
static const float WAVE_SPEED = 1.0;
static const ofVec3f CENTER = ofVec3f(0, -MESHED_HEIGHT_SIZE/2, -MESHED_HEIGHT_SIZE/2);
static bool debugMode = false;
bool started = false;

ofApp::ofApp() {
    space = new Space(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 150.0, 75, "space3.jpg");
    floor = new Floor(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 5, space->getSpaceMesh().getColors(), WAVE_SPEED);
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

    for (auto it = dots.begin(); it < dots.end(); it++) {
        delete *it;
    }
    for (auto it = ribbons.begin(); it < ribbons.end(); it++) {
        delete *it;
    }
}

void ofApp::setup(){
    ofDisableArbTex();
    //dots.png
    ofLoadImage(dotsTexture, "dot.png");
    ofEnableArbTex();
    
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
    invertShader.load("invertShaderVert.c", "invertShaderFrag.c");
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 0);
    postGlitch.setup(&fbo);
    

    targetUpVec = ofVec3f(0,1,0);
    currentUpVec = targetUpVec;
    camPosition = ofVec3f(0, -400, 0);
    currentLookAt = CENTER;
    targetLookAt = currentLookAt;
    

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
    
    
    // position
    ofVec3f move = camPosition - cam.getPosition();
    if (move.length() > CAM_SPEED) {
        move.normalize();
        move *= CAM_SPEED;
    }
    
    // look at
    ofVec3f lookAtDiff = targetLookAt - currentLookAt;
    if (lookAtDiff.length() > LOOK_AT_SPEED) {
        lookAtDiff.normalize();
        lookAtDiff *= LOOK_AT_SPEED;
        currentLookAt += lookAtDiff;
    } else if (started) {
        setRandomLookAt();
    }
    
    // updir
    if (currentUpVec.x < targetUpVec.x - ROLL_SPEED) {
        currentUpVec.x += ROLL_SPEED;
        
    } else if (currentUpVec.x > targetUpVec.x + ROLL_SPEED) {
        currentUpVec.x -= ROLL_SPEED;
    }
    if (currentUpVec.y < targetUpVec.y - ROLL_SPEED) {
        currentUpVec.y += ROLL_SPEED;
    } else if (currentUpVec.y > targetUpVec.y + ROLL_SPEED) {
        currentUpVec.y -= ROLL_SPEED;
    }
    
    cam.move(move);
    cam.lookAt(currentLookAt, currentUpVec.getNormalized());
    
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
    
    if (debugMode) ofDrawBox(currentLookAt, 20);
    // matrix stuff
    ofPushMatrix();
    ofTranslate(-MESHED_WIDTH_SIZE/2, -MESHED_HEIGHT_SIZE/2);
    ofRotateDeg(90, -1, 0, 0);
    
    // draw floor
    floorShader.begin();
    floorShader.setUniform1f("value", floor->getWaveValue());
    floor->draw();
    floorShader.end();
    
    // draw space
    spaceShader.begin();
    spaceShader.setUniform1f("value", space->getJitterValue());
    space->draw();
    spaceShader.end();
    
    // draw space
    invertShader.begin();
    invertShader.setUniform1f("value", space->getJitterValue());
    space->draw();
    invertShader.end();
    
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
    dotShader.setUniform1f("ellapsedTime", ofGetElapsedTimef());
    for (auto it = dots.begin(); it < dots.end(); it++) {
        (*it)->draw();
    }
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
    
    if (debugMode) {
        ofSetColor(ofColor::green);
        ofDrawBitmapString("Position : " + ofToString(cam.getPosition()), 100, 100);
        ofDrawBitmapString("Up axis : " + ofToString(cam.getUpAxis()), 100, 120);
        ofDrawBitmapString("Target : " + ofToString(cam.getTarget().getPosition()), 100, 140);
        ofDrawBitmapString("Roll deg : " + ofToString(cam.getRollDeg()), 100, 160);
        ofDrawBitmapString("Pitch deg : " + ofToString(cam.getPitchDeg()), 100, 180);
        ofDrawBitmapString("Heading deg : " + ofToString(cam.getHeadingDeg()), 100, 200);
        ofDrawBitmapString("Current upvec : " + ofToString(currentUpVec), 100, 220);
        ofDrawBitmapString("Target upVec : " + ofToString(targetUpVec), 100, 240);
        ofDrawBitmapString("Current lookat : " + ofToString(currentLookAt), 100, 260);
        ofDrawBitmapString("Target lookat : " + ofToString(targetLookAt), 100, 280);
    }
}

void ofApp::setRandomLookAt() {
    int x = (int) (ofRandom(CAM_MAX_X * 2)-CAM_MAX_X) * LOOK_AT_SCALE;
    int y = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * LOOK_AT_SCALE;
    int z = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * LOOK_AT_SCALE;
    targetLookAt = CENTER + ofVec3f(x, y, z);
}

void ofApp::setRandomCamPosition(){
    started = true;
    int x = (int) ofRandom(CAM_MAX_X * 2)-CAM_MAX_X;
    int y = (int) ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z;
    int z = (int) ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z;
    camPosition = CENTER + ofVec3f(x, y, z);
    
    setRandomLookAt();
    
    targetUpVec = ofVec3f(ofRandom(2) - 1, ofRandom(2) - 1, 0).normalize();
}

void ofApp::addRibbon(){
    ribbons.push_back(new Ribbon(ofPoint(ofRandom(MESHED_WIDTH_SIZE), ofRandom(MESHED_WIDTH_SIZE), ofRandom(MESHED_WIDTH_SIZE) - MESHED_WIDTH_SIZE/2), ofVec3f(0, 5, 0), 5.0, MESHED_WIDTH_SIZE, MESHED_HEIGHT_SIZE, MESHED_WIDTH_SIZE, 500));
}

void ofApp::addDots(){
    dots.push_back(new Dots(30, 600, ofPoint(ofRandom(MESHED_WIDTH_SIZE), ofRandom(MESHED_WIDTH_SIZE), ofRandom(MESHED_WIDTH_SIZE) - MESHED_WIDTH_SIZE/2),dotsTexture));
}

void ofApp::changeRibbonsDirection() {
    for (auto it = ribbons.begin(); it < ribbons.end(); it++) {
        (*it)->changeDirection();
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
            changeRibbonsDirection();
            break;
        case 'x':
            addRibbon();
            break;
        case 'c':
            addDots();
            break;
        case 'l':
            setRandomCamPosition();
            break;
        case 'k':
            cam.lookAt(CENTER);
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
    // kick
    if (eventArgs.channel == 1){
        postGlitch.setFx(OFXPOSTGLITCH_SHAKER, eventArgs.status == MidiStatus::MIDI_NOTE_ON);
    }
    
    // snare
    if (eventArgs.channel == 2){
        postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, eventArgs.status == MidiStatus::MIDI_NOTE_ON);
    }
    
    // wave
    if (eventArgs.channel == 3 && eventArgs.status == MidiStatus::MIDI_NOTE_ON){
        floor->setTargetWaveValue(eventArgs.velocity == 1 ? 0 : eventArgs.velocity);
    }
    
    //direction
    if (eventArgs.channel == 4 && eventArgs.status == MidiStatus::MIDI_NOTE_ON){
        setRandomCamPosition();
    }
    
    // dots
    if (eventArgs.channel == 5 && eventArgs.status == MidiStatus::MIDI_NOTE_ON){
        addDots();
    }
    
    // ribbons
    if (eventArgs.channel == 6 && eventArgs.status == MidiStatus::MIDI_NOTE_ON){
        addRibbon();
    }
    
    // ribbons direction
    if (eventArgs.channel == 7 && eventArgs.status == MidiStatus::MIDI_NOTE_ON){
        changeRibbonsDirection();
    }
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
