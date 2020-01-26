#include "ofApp.h"

static const int TRIANGLE_SIZE = 10;
static const int MESH_WIDTH = 100;
static const int MESH_HEIGHT = 250;
static const int FBO_SAMPLES = 0;
float startTime = 0;
float waveValue = 0;
float waveStartTime = 0.0;
float jitterValue = 0;
float jitterStartTime = 0.0;
float intensityThreshold = 130.0;
float connectionDistance = 100;
int trianglesVectorSize = 5;
int pointsCount = 50;
int maxLinesCount = 1000;
int lineSpeed = 5;


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
    
    ofDisableArbTex();
    ofLoadImage(dotsTexture, "dot.png");
    ofEnableArbTex();
    
    // init objects
    initMeshes(MESH_WIDTH, MESH_HEIGHT);
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, FBO_SAMPLES);
    postGlitch.setup(&fbo);
    
    startTime = ofGetElapsedTimef();
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
    
    if (waveValue > 0) {
        waveValue -= ofGetElapsedTimef() - waveStartTime;
    }
    
    if (jitterValue > 0) {
        jitterValue -= ofGetElapsedTimef() - jitterStartTime;
    }
    
//    postGlitch.setFx(OFXPOSTGLITCH_NOISE,true);
//    postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER,true);
//    postGlitch.setFx(OFXPOSTGLITCH_GLOW,true);
    
    //cam.enableInertia();
    cam.move(0, 0, -1 * (ofGetElapsedTimef()/3.0f));
    //cam.rotateDeg(ofNoise(ofGetElapsedTimef()) - 0.5, 0, 0, 10);
    
    updatesLines();
    
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
    floorShader.setUniform1f("value", waveValue);
    floorShader.setUniform1f("cameraZ", cam.getZ());
    floorShader.setUniform1f("meshHeight", MESH_HEIGHT * TRIANGLE_SIZE);
    floorMesh.drawWireframe();
    for (int i = 0; i < trianglesVectorSize; i++){
        //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMeshesActivation[i]));
        if (trianglesMeshesActivation[i]){
            trianglesMeshes[i].draw();
        }
    }
    floorShader.end();

    // draw space
    spaceShader.begin();
    spaceShader.setUniform1f("value", jitterValue);
    spaceMesh.draw();
    spaceShader.end();
    
    // draw lines
    ofSetLineWidth(5);
    ofSetColor(ofColor::red);
    linesMesh.draw();
    ofSetLineWidth(1);

    // draw dots
    glDepthMask(GL_FALSE);
    ofSetColor(255, 100, 90);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    dotShader.begin();
    dotsTexture.bind();
    dotsMesh.draw();
    dotsTexture.unbind();
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
void ofApp::initMeshes(int width, int height) {
    createSpaceMesh();
    createFloorMesh(height, width);
    createDotsMesh();
    
    //lines mesh
    lineDirection = ofVec3f(0, lineSpeed, 0);
    lastLinePoint = ofPoint(200, 0, 200);
    linesMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
}
void ofApp::createSpaceMesh() {
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

void ofApp::createFloorMesh(int height, int width, int triangleSize) {
    vector<int> triangleMeshesIndexes;
    // triangles and floor meshes
    for (int i = 0; i < trianglesVectorSize; i++){
        auto tempMesh = ofMesh();
        tempMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
        trianglesMeshes.push_back(tempMesh);
        triangleMeshesIndexes.push_back(0);
        trianglesMeshesActivation.push_back(false);
    }
    
    
    floorMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    int index = 0;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int pan = j % 2 == 0 ? 0 : triangleSize / 2;
            
            int x, y, z = 0;
            
            x = i * triangleSize + pan;
            y = j * triangleSize;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point1 = ofPoint(x, y, z);
            
            x = (i + 0.5 ) * triangleSize + pan;
            y = (j + 1) * triangleSize;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point2 = ofPoint(x, y, z);
            
            x = (i + 1) * triangleSize + pan;
            y = j * TRIANGLE_SIZE;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point3 = ofPoint(x, y, z);
            
            x = (i + 1.5) * triangleSize + pan;
            y = (j + 1) * triangleSize;
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
                int meshIdx = (int) ofRandom(trianglesVectorSize - 0.1);
                ofMesh& trianglesMesh = trianglesMeshes[meshIdx];
                //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMesh.getVertices().size()));
                int trianglesIndex = triangleMeshesIndexes[meshIdx];
                
                trianglesMesh.addVertex(point1);
                trianglesMesh.addColor(spaceMesh.getColors().at(ofRandom(spaceMesh.getColors().size())));
                trianglesMesh.addVertex(point2);
                trianglesMesh.addColor(spaceMesh.getColors().at(ofRandom(spaceMesh.getColors().size())));
                trianglesMesh.addVertex(point3);
                trianglesMesh.addColor(spaceMesh.getColors().at(ofRandom(spaceMesh.getColors().size())));
                trianglesMesh.addIndex(trianglesIndex);
                trianglesMesh.addIndex(trianglesIndex + 1);
                trianglesMesh.addIndex(trianglesIndex + 2);
                
                if (ofRandom(1) > 0.75) {
                    trianglesMesh.addVertex(point4);
                    trianglesMesh.addColor(spaceMesh.getColors().at(ofRandom(spaceMesh.getColors().size())));
                    trianglesMesh.addIndex(trianglesIndex + 1);
                    trianglesMesh.addIndex(trianglesIndex + 2);
                    trianglesMesh.addIndex(trianglesIndex + 3);
                    trianglesIndex +=  1;
                }
                trianglesIndex +=  3;
                triangleMeshesIndexes[meshIdx] = trianglesIndex;
                //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMesh.getVertices().size()));
            }
        }
    }
    
    return floorMesh;
}

void ofApp::createDotsMesh() {
    dotsMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
    for (int i = 0; i < pointsCount; i++){
        dotsMesh.addVertex(ofPoint(500 + ofRandom(200), 500 + ofRandom(200), 500 + ofRandom(200)));
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
        case 'a':
            triggerTriangles(0);
            break;
        case 's':
            triggerTriangles(1);
            break;
        case 'd':
            triggerTriangles(2);
            break;
        case 'f':
            triggerTriangles(3);
            break;
        case 'g':
            triggerTriangles(4);
            break;
        case 'z':
            changeLineDirection();
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

void ofApp::startWave(){
    waveValue = 5;
    waveStartTime = ofGetElapsedTimef();
}

void ofApp::startJitter(){
    jitterValue = 2;
    jitterStartTime = ofGetElapsedTimef();
}

void ofApp::triggerTriangles(int i){
    trianglesMeshesActivation[i] = trianglesMeshesActivation[i] ? false : true;
}

void ofApp::changeLineDirection(){
    float x = rand() % 3 - 1;
    float y = 1;
    float z = rand() % 3 - 1;
    lineDirection = ofVec3f(x, y, z);
    //lineDirection.normalize();
    lineDirection *= lineSpeed;
    lineDirection.y = lineSpeed;
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(lineDirection));
}

void ofApp::updatesLines() {
    if (lastLinePoint.x + lineDirection.x < 0 || lastLinePoint.x + lineDirection.x > MESH_WIDTH * TRIANGLE_SIZE) {
        lineDirection.x = -lineDirection.x;
    }
    if (lastLinePoint.z + lineDirection.z > 500 || lastLinePoint.z + lineDirection.z < -500) {
        lineDirection.z = -lineDirection.z;
    }
    
    int x = lastLinePoint.x + lineDirection.x;
    int y = lastLinePoint.y + lineDirection.y;
    int z = lastLinePoint.z + lineDirection.z;
    
    ofPoint newPoint = ofPoint(x, y, z);
    lastLinePoint = newPoint;
    linesMesh.addVertex(newPoint);
    if (linesMesh.getVertices().size() > maxLinesCount) {
        linesMesh.getVertices().erase(linesMesh.getVertices().begin());
    }
}
