#include "ofApp.h"

float startTime = 0;

float waveValue = 0;
float waveStartTime = 0.0;

float jitterValue = 0;
float jitterStartTime = 0.0;

//int imageSize = 300;
float intensityThreshold = 130.0;
float connectionDistance = 100;

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
    initSphere();
    initSpaceMesh();
    floorShader.load("floorShaderVert.c", "floorShaderFrag.c");
    spaceShader.load("spaceShaderVert.c", "spaceShaderFrag.c");
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<BloomPass>()->setEnabled(true);
    startTime = ofGetElapsedTimef();
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
    
    if (jitterValue > 0) {
        jitterValue -= ofGetElapsedTimef() - jitterStartTime;
    }
    
//    cam.enableInertia();
//    cam.move(0, 0, -1 * (ofGetElapsedTimef()/5.0f));
//    cam.rotateDeg(ofNoise(ofGetElapsedTimef()) - 0.5, 0, 0, 10);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(ofColor::black);
    //ofBackground(0,0,255);
    //cam.begin();
    post.begin(cam);
    
    ofPushMatrix();

    // matrix stuff
//    ofTranslate(-(MESH_WIDTH*TRIANGLE_SIZE)/2, -ofGetHeight()/2);
//    ofRotateDeg(90, -1, 0, 0);
    
    
    floorShader.begin();
    floorShader.setUniform1f("value", waveValue);
    floorShader.setUniform1f("cameraZ", cam.getZ());
    //ofLog(OF_LOG_NOTICE, ofToString(cam.getZ()));
    floorShader.setUniform1f("meshHeight", MESH_HEIGHT * TRIANGLE_SIZE);
    floorMesh.drawWireframe();
    floorShader.end();

    spaceShader.begin();
    spaceShader.setUniform1f("value", jitterValue);
    spaceMesh.draw();
    spaceShader.end();
    
    //sphere.drawWireframe();
    //spaceImage.draw(0, 0);


    ofPopMatrix();
    
    //cam.end();
    post.end();
    
    // capture the image if recording is started
    // this can slow down the rendering by a lot, so be aware of the framerate...
    // if frames are skipped this is going to mess up the rendering when imported as a video
    if (recording){
        screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        recorder.addFrame(screenCapture);
    }
}

void ofApp::initMesh(int width, int height) {
    floorMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    int index = 0;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int pan = j % 2 == 0 ? 0 : TRIANGLE_SIZE / 2;
            
            floorMesh.addVertex(ofPoint(i * TRIANGLE_SIZE + pan, j * TRIANGLE_SIZE));
            floorMesh.addVertex(ofPoint((i + 0.5 ) * TRIANGLE_SIZE + pan, (j + 1) * TRIANGLE_SIZE));
            floorMesh.addVertex(ofPoint((i + 1) * TRIANGLE_SIZE + pan, j * TRIANGLE_SIZE));
            floorMesh.addVertex(ofPoint((i + 1.5) * TRIANGLE_SIZE + pan, (j + 1) * TRIANGLE_SIZE));
            
            floorMesh.addIndex(index);
            floorMesh.addIndex(index + 1);
            floorMesh.addIndex(index + 2);
            floorMesh.addIndex(index + 1);
            floorMesh.addIndex(index + 2);
            floorMesh.addIndex(index + 3);
            
            // add noise
            for (int i = 0; i < 4; i++){
                auto vertex = (ofPoint) floorMesh.getVertex(index + i);
                vertex.z = (ofNoise(vertex.x, vertex.y) - 0.5) * 20;
                floorMesh.setVertex(index + i, vertex);
            }
            
            index += 4;
        }
    }
}

void ofApp::initSphere() {
    sphere = ofSpherePrimitive(100, 50).getMesh();
}

void ofApp::initSpaceMesh() {
    spaceImage.load("space3.jpg");
    spaceImage.resize(MESH_WIDTH*2, MESH_HEIGHT*2);
    spaceMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
    spaceMesh.enableColors();
    spaceMesh.enableIndices();
    
    int w = spaceImage.getWidth();
    int h = spaceImage.getHeight();
    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
            ofColor c = spaceImage.getColor(x, y);
            float intensity = c.getLightness();
            if (intensity >= intensityThreshold) {
                ofVec3f pos(x * (MESH_WIDTH*TRIANGLE_SIZE / spaceImage.getWidth()), y * (MESH_HEIGHT*TRIANGLE_SIZE / spaceImage.getHeight()), 300 + ofNoise(x, y) * 50);
                spaceMesh.addVertex(pos);
                spaceMesh.addColor(c);
            }
        }
    }
    
    int numVerts = spaceMesh.getNumVertices();
    for (int a=0; a<numVerts; ++a) {
        ofVec3f verta = spaceMesh.getVertex(a);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = spaceMesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance <= connectionDistance) {
                spaceMesh.addIndex(a);
                spaceMesh.addIndex(b);
            }
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
            break;
        case 'j':
            startJitter();
            break;
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

void ofApp::startJitter(){
    jitterValue = 2;
    jitterStartTime = ofGetElapsedTimef();
}

