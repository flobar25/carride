//
//  Floor.cpp
//  carrideOF
//
//  Created by Florent Bariod on 1/26/20.
//
#include "ofMain.h"
#include "Floor.h"

Floor::Floor(int height, int width, int trianglesSize, int trianglesSetCount, vector<ofColor_<float> >& colors) {
    // keeping track of the different meshes indexes
    vector<int> triangleMeshesIndexes;
    
    //init triangles meshes
    for (int i = 0; i < trianglesSetCount; i++){
        auto tempMesh = ofMesh();
        tempMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
        trianglesMeshes.push_back(tempMesh);
        triangleMeshesIndexes.push_back(0);
        tranglesActivation.push_back(false);
    }
    
    floorMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    int index = 0;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++) {
            int pan = j % 2 == 0 ? 0 : trianglesSize / 2;
            
            int x, y, z = 0;
            
            x = i * trianglesSize + pan;
            y = j * trianglesSize;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point1 = ofPoint(x, y, z);
            
            x = (i + 0.5 ) * trianglesSize + pan;
            y = (j + 1) * trianglesSize;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point2 = ofPoint(x, y, z);
            
            x = (i + 1) * trianglesSize + pan;
            y = j * trianglesSize;
            z = (ofNoise(x, y) - 0.5) * 20;
            ofPoint point3 = ofPoint(x, y, z);
            
            x = (i + 1.5) * trianglesSize + pan;
            y = (j + 1) * trianglesSize;
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
                int meshIdx = (int) ofRandom(trianglesSetCount - 0.1);
                ofMesh& trianglesMesh = trianglesMeshes[meshIdx];
                //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMesh.getVertices().size()));
                int trianglesIndex = triangleMeshesIndexes[meshIdx];
                
                trianglesMesh.addVertex(point1);
                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
                trianglesMesh.addVertex(point2);
                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
                trianglesMesh.addVertex(point3);
                trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
                trianglesMesh.addIndex(trianglesIndex);
                trianglesMesh.addIndex(trianglesIndex + 1);
                trianglesMesh.addIndex(trianglesIndex + 2);
                
                if (ofRandom(1) > 0.75) {
                    trianglesMesh.addVertex(point4);
                    trianglesMesh.addColor(colors.at(ofRandom(colors.size())));
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
    
    waveValue = 0;
}

void Floor::toggleTriangles(int i) {
    tranglesActivation[i] = tranglesActivation[i] ? false : true;
}

void Floor::startWave() {
    waveValue = 5;
    waveStartTime = ofGetElapsedTimef();
}

void Floor::update() {
    if (waveValue > 0) {
        waveValue -= ofGetElapsedTimef() - waveStartTime;
    }
}

void Floor::draw() {
    ofSetColor(ofColor::white);
    floorMesh.drawWireframe();
    for (int i = 0; i < trianglesMeshes.size(); i++){
        //ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(trianglesMeshesActivation[i]));
        if (tranglesActivation[i]){
            trianglesMeshes[i].draw();
        }
    }
}

float Floor::getWaveValue() {
    return waveValue;
}
