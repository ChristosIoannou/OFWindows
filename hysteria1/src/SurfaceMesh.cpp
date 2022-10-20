#include "SurfaceMesh.h"

void SurfaceMesh::setup() {
    //cam.setDistance(100);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            mesh.addVertex(ofPoint((x - size / 2)*6, (y - size / 2)*6));
        }
    }

    for (int x = 0; x < size - 1; x++) {
        for (int y = 0; y < size - 1; y++) {
            mesh.addIndex(x + y * size);
            mesh.addIndex((x + 1) + y * size);
            mesh.addIndex(x + (y + 1) * size);
            mesh.addIndex((x + 1) + y * size);
            mesh.addIndex((x + 1) + (y + 1) * size);
            mesh.addIndex(x + (y + 1) * size);
        }
    }
}

void SurfaceMesh::update(float bass, float mids, float highs) {
    ofSetFrameRate(-1);
    int frameNum = ofGetFrameNum();
    int count = 0;
    mesh.clearColors();
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            ofVec3f position = mesh.getVertex(count);
            if (usePerlin)
                position.z = (ofMap(ofSignedNoise(count, ofGetElapsedTimef()), -1, 1, 0, bass*20));
            else
                position.z = (ofMap(ofNoise(count, ofGetElapsedTimef()), 0, 1, 0, bass * 20));
            //std::cout << position.z << std::endl;
            // ofColor SurfaceMesh::surfaceColorRadiant(ofVec3f position)
            mesh.setVertex(count, position);
            mesh.addColor(calculateColor(position));
            count++;
        }
    }
}

void SurfaceMesh::draw() {
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    cam.begin();
    //ofColor surfaceColor;
    //surfaceColor.setHsb(145, 255, 255, 70);
    //ofSetColor(surfaceColor);
    mesh.drawWireframe();
    cam.end();
    ofPopMatrix();
    //gui.draw();
}

ofFloatColor SurfaceMesh::calculateColor(ofVec3f position) {
    ofFloatColor surfaceColor;
   // auto position = position_;

    switch (colorScheme)
    {
    case ColorScheme::Z:
        surfaceColor.setHsb(ofMap(position.z, 0, 30, 0.392, 0.588, false), 1.0f, 1.0f, 0.392);
        //surfaceColor.setHsb(ofMap(position.z, 0, 30, 100, 150, false), 255, 255, 105);
        break;
    case ColorScheme::RAD:
        surfaceColor.setHsb(1.0f, 1.0f, 1.0f, 1.0f);
    }

    return surfaceColor;
}