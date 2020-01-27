#include "ofApp.h"

static const int TRIANGLE_SIZE = 10;
static const int MESH_WIDTH = 100;
static const int MESH_HEIGHT = 250;


ofApp::ofApp() {
    space = new Space(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 130.0, 100, "space3.jpg");
    floor = new Floor(MESH_HEIGHT, MESH_WIDTH, TRIANGLE_SIZE, 5, space->getSpaceMesh().getColors());
    dots = new Dots(50, 200, ofPoint(500, 500, 500), "dot.png");
    ribbon = new Ribbon(ofPoint(200, 0, 200), ofVec3f(0, 5, 0), 5.0, MESH_WIDTH * TRIANGLE_SIZE, 200, 500);
    
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
    delete ribbon;
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
    
    ribbon->update();
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
    ribbon->draw();

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
//
//Space* ofApp::createSpace(int height, int width, int trianglesSize, string imagePath) {
//    ofImage spaceImage;
//    spaceImage.load(imagePath);
//    spaceImage.resize(width*2, height*2);
//    
//    ofMesh spaceMesh;
//    spaceMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
//    spaceMesh.enableColors();
//    spaceMesh.enableIndices();
//    
//    int w = spaceImage.getWidth();
//    int h = spaceImage.getHeight();
//    for (int x=0; x<w; ++x) {
//        for (int y=0; y<h; ++y) {
//            ofColor c = spaceImage.getColor(x, y);
//            float intensity = c.getLightness();
//            if (intensity >= intensityThreshold) {
//                ofVec3f pos(x * (width*trianglesSize / spaceImage.getWidth()), y * (width*trianglesSize / spaceImage.getHeight()), 300 + ofNoise(x, y) * 50);
//                spaceMesh.addVertex(pos);
//                spaceMesh.addColor(c);
//            }
//        }
//    }
//    
//    int numVerts = spaceMesh.getNumVertices();
//    for (int a=0; a<numVerts; ++a) {
//        ofVec3f verta = spaceMesh.getVertex(a);
//        for (int b=a+1; b<numVerts; ++b) {
//            ofVec3f vertb = spaceMesh.getVertex(b);
//            float distance = verta.distance(vertb);
//            if (distance <= connectionDistance) {
//                spaceMesh.addIndex(a);
//                spaceMesh.addIndex(b);
//            }
//        }
//    }
//    
//    return new Space(spaceMesh);
//}

//Floor ofApp::createFloor(int height, int width, int trianglesSize, vector<ofColor> colors) {
//    // multiple triangles meshes to activate/deactivate separate set of triangles easily
//    vector<ofMesh> trianglesMeshes;
//    // keeping track of the different meshes indexes
//    vector<int> triangleMeshesIndexes;
//    
//    //init triangles meshes
//    for (int i = 0; i < trianglesVectorSize; i++){
//        auto tempMesh = ofMesh();
//        tempMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
//        trianglesMeshes.push_back(tempMesh);
//        triangleMeshesIndexes.push_back(0);
//    }
//    
//    ofMesh floorMesh;
//    floorMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
//    int index = 0;
//    for (int i = 0; i < width; i++){
//        for (int j = 0; j < height; j++){
//            int pan = j % 2 == 0 ? 0 : trianglesSize / 2;
//            
//            int x, y, z = 0;
//            
//            x = i * trianglesSize + pan;
//            y = j * trianglesSize;
//            z = (ofNoise(x, y) - 0.5) * 20;
//            ofPoint point1 = ofPoint(x, y, z);
//            
//            x = (i + 0.5 ) * trianglesSize + pan;
//            y = (j + 1) * trianglesSize;
//            z = (ofNoise(x, y) - 0.5) * 20;
//            ofPoint point2 = ofPoint(x, y, z);
//            
//            x = (i + 1) * trianglesSize + pan;
//            y = j * trianglesSize;
//            z = (ofNoise(x, y) - 0.5) * 20;
//            ofPoint point3 = ofPoint(x, y, z);
//            
//            x = (i + 1.5) * trianglesSize + pan;
//            y = (j + 1) * trianglesSize;
//            z = (ofNoise(x, y) - 0.5) * 20;
//            ofPoint point4 = ofPoint(x, y, z);
//            
//            
//            floorMesh.addVertex(point1);
//            floorMesh.addVertex(point2);
//            floorMesh.addVertex(point3);
//            floorMesh.addVertex(point4);
//            
//            floorMesh.addIndex(index);
//            floorMesh.addIndex(index + 1);
//            floorMesh.addIndex(index + 2);
//            floorMesh.addIndex(index + 1);
//            floorMesh.addIndex(index + 2);
//            floorMesh.addIndex(index + 3);
//            index += 4;
//            
//            // add some random triangles
//            if (ofRandom(1) > 0.75) {
//                int meshIdx = (int) ofRandom(trianglesVectorSize - 0.1);
//                ofMesh& trianglesMesh = trianglesMeshes[meshIdx];
//                //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMesh.getVertices().size()));
//                int trianglesIndex = triangleMeshesIndexes[meshIdx];
//                
//                trianglesMesh.addVertex(point1);
//                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
//                trianglesMesh.addVertex(point2);
//                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
//                trianglesMesh.addVertex(point3);
//                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
//                trianglesMesh.addIndex(trianglesIndex);
//                trianglesMesh.addIndex(trianglesIndex + 1);
//                trianglesMesh.addIndex(trianglesIndex + 2);
//                
//                if (ofRandom(1) > 0.75) {
//                    trianglesMesh.addVertex(point4);
//                    trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
//                    trianglesMesh.addIndex(trianglesIndex + 1);
//                    trianglesMesh.addIndex(trianglesIndex + 2);
//                    trianglesMesh.addIndex(trianglesIndex + 3);
//                    trianglesIndex +=  1;
//                }
//                trianglesIndex +=  3;
//                triangleMeshesIndexes[meshIdx] = trianglesIndex;
//                //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMesh.getVertices().size()));
//            }
//        }
//    }
//    
//    return Floor(Floor(floorMesh, trianglesMeshes));
//}


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
            ribbon->changeDirection();
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
