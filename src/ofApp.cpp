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
    
//    // fbo (needed to reduce aliasing)
//    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
    
    // init objects
    initMesh(MESH_WIDTH, MESH_HEIGHT);
    initSphere();
    initSpaceMesh();
    
    // post processing
    post.init(ofGetWidth(), ofGetHeight(), 4);
    post.createPass<BloomPass>()->setEnabled(true);
    post.setFlip(false);
    
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
//        cam.move(0, 0, -1 * (ofGetElapsedTimef()/5.0f));
//        cam.rotateDeg(ofNoise(ofGetElapsedTimef()) - 0.5, 0, 0, 10);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // setup gl state
    ofEnableDepthTest();
    //light.enable();
    
    post.begin(cam);
    ofClear(0,0,0,255);
    ofSetLineWidth(1);
    
    // matrix stuff
    ofPushMatrix();
    ofTranslate(-(MESH_WIDTH*TRIANGLE_SIZE)/2, -ofGetHeight()/2);
    ofRotateDeg(90, -1, 0, 0);
    
    // draw stuff
    //floor
    floorShader.begin();
    floorShader.setUniform1f("value", waveValue);
    floorShader.setUniform1f("cameraZ", cam.getZ());
    floorShader.setUniform1f("meshHeight", MESH_HEIGHT * TRIANGLE_SIZE);
    floorMesh.drawWireframe();
    ofSetColor(ofColor::red);
    trianglesMesh.draw();
    floorShader.end();
    
    //space
    spaceShader.begin();
    spaceShader.setUniform1f("value", jitterValue);
    spaceMesh.draw();
    spaceShader.end();
    
    
    // end
    ofPopMatrix();
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
    trianglesMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    floorMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    int trianglesIndex = 0;
    int index = 0;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int pan = j % 2 == 0 ? 0 : TRIANGLE_SIZE / 2;
            
            int x, y, z = 0;
            
            x = i * TRIANGLE_SIZE + pan;
            y = j * TRIANGLE_SIZE;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point1 = ofPoint(x, y, z);

            x = (i + 0.5 ) * TRIANGLE_SIZE + pan;
            y = (j + 1) * TRIANGLE_SIZE;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point2 = ofPoint(x, y, z);
            
            x = (i + 1) * TRIANGLE_SIZE + pan;
            y = j * TRIANGLE_SIZE;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point3 = ofPoint(x, y, z);
            
            x = (i + 1.5) * TRIANGLE_SIZE + pan;
            y = (j + 1) * TRIANGLE_SIZE;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point4 = ofPoint(x, y, z);
            
            
            floorMesh.addVertex(point1);
            floorMesh.addVertex(point2);
            floorMesh.addVertex(point3);
            floorMesh.addVertex(point4);

            floorMesh.addIndex(index);
            floorMesh.addIndex(index + 1);
            floorMesh.addIndex(index + 2);
            floorMesh.addIndex(index + 1);
            floorMesh.addIndex(index + 2);
            floorMesh.addIndex(index + 3);
            index += 4;
            
            // add some random triangles
            if (ofRandom(1) > 0.75) {
                trianglesMesh.addVertex(point1);
                trianglesMesh.addVertex(point2);
                trianglesMesh.addVertex(point3);
                trianglesMesh.addIndex(trianglesIndex);
                trianglesMesh.addIndex(trianglesIndex + 1);
                trianglesMesh.addIndex(trianglesIndex + 2);
                
                if (ofRandom(1) > 0.75) {
                    trianglesMesh.addVertex(point4);
                    trianglesMesh.addIndex(trianglesIndex + 1);
                    trianglesMesh.addIndex(trianglesIndex + 2);
                    trianglesMesh.addIndex(trianglesIndex + 3);
                    trianglesIndex += 1;
                }
                trianglesIndex += 3;
            }
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

