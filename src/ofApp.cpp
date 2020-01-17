#include "ofApp.h"

float waveValue = 0;
float waveStartTime = 0.0;
float waveLength = 0.0;


//--------------------------------------------------------------
void ofApp::setup(){
    recorder.setPrefix(ofToDataPath("recording/frame_"));
    recorder.setFormat("bmp");
    recorder.startThread();
    ofSetFrameRate(30);
    midiIn.openPort(0);
    midiIn.addListener(this);
    ofEnableDepthTest();
    initMesh(MESH_WIDTH, MESH_HEIGHT);
    shader.load("shaderVert.c", "shaderFrag.c");
}

void ofApp::exit(){
    recorder.waitForThread();
    recorder.stopThread();
}

//--------------------------------------------------------------
void ofApp::update(){
    // show the framerate on window title
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    if (waveValue > 0) {
        waveValue -= ofGetElapsedTimef() - waveStartTime;
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(ofColor::black);
    cam.begin();
    ofPushMatrix();
    
    //shader stuff
    shader.begin();
    shader.setUniform1f("value", waveValue);
    
    // matrix stuff
    ofTranslate(-(MESH_WIDTH*TRIANGLE_SIZE)/2, -ofGetHeight()/2);
    ofRotateDeg(20, -1, 0, 0);
    
    // draw
    mesh.drawWireframe();
    
    //closing stuff
    shader.end();
    ofPopMatrix();
    cam.end();
    
    // capture the image if recording is started
    // this can slow down the rendering by a lot, so be aware of the framerate...
    // if frames are skipped this is going to mess up the rendering when imported as a video
    if (recording){
        screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        recorder.addFrame(screenCapture);
    }
}

void ofApp::initMesh(int width, int height) {
    mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    int index = 0;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int pan = j % 2 == 0 ? 0 : TRIANGLE_SIZE / 2;
            
            mesh.addVertex(ofPoint(i * TRIANGLE_SIZE + pan, j * TRIANGLE_SIZE));
            mesh.addVertex(ofPoint((i + 0.5 ) * TRIANGLE_SIZE + pan, (j + 1) * TRIANGLE_SIZE));
            mesh.addVertex(ofPoint((i + 1) * TRIANGLE_SIZE + pan, j * TRIANGLE_SIZE));
            mesh.addVertex(ofPoint((i + 1.5) * TRIANGLE_SIZE + pan, (j + 1) * TRIANGLE_SIZE));
            
            mesh.addIndex(index);
            mesh.addIndex(index + 1);
            mesh.addIndex(index + 2);
            mesh.addIndex(index + 1);
            mesh.addIndex(index + 2);
            mesh.addIndex(index + 3);
            
            // add noise
            for (int i = 0; i < 4; i++){
                auto vertex = (ofPoint) mesh.getVertex(index + i);
                vertex.z = (ofNoise(vertex.x, vertex.y) - 0.5) * 20;
                mesh.setVertex(index + i, vertex);
            }
            
            index += 4;
        }
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
            startWave();
        default:
            break;
    }
}

void ofApp::newMidiMessage(ofxMidiMessage& eventArgs){
    
}

void ofApp::captureScreen(){
    screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    screenCapture.save("captures/capture.bmp");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
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

void ofApp::startWave(){
    waveValue = 5;
    waveStartTime = ofGetElapsedTimef();
}

