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

    XstartOffset = 0.0f;
    YstartOffset = 0.0f;
    ZstartOffset = 0.0f;
    //XangleIncrement = 180.0 / float(size);
    //YangleIncrement = 180.0 / float(size);
    ZangleIncrement = 180.0 / float(size);
}

void SurfaceMesh::update(float bass, float mids, float highs) {
    ofSetFrameRate(-1);
    int frameNum = ofGetFrameNum();
    int count = 0;
    mesh.clearColors();
    // We calculate circle colour limits outside of loop for speed since they are not vertex dependent
    if (sinCircle)
    {
        circleLower = 0.5 * (1 + sin(ofGetFrameNum() * 0.05));
        circleUpper = 0.5 + (1 + cos(ofGetFrameNum() * 0.04));
    }
    else {
        circleLower = 0.0;
        circleUpper = 1.0f;
    }
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

    if (zoom)
        ofTranslate(0, 0, (abs(ofSignedNoise(ofGetElapsedTimef()) * 0.1)) * 1000);
    if (rotateZ)
        ofRotateZ(ofSignedNoise(ofGetElapsedTimef() * 0.1) * 720.0);
    if (rotateX)
        ofRotateX(ofSignedNoise(ofGetElapsedTimef() * 0.1) * 720.0);
    if (rotateY)
        ofRotateY(ofSignedNoise(ofGetElapsedTimef() * 0.1) * 720.0);

    mesh.drawWireframe();
    cam.end();
    ofPopMatrix();
    //gui.draw();
}

ofFloatColor SurfaceMesh::calculateColor(ofVec3f position) {
    ofFloatColor surfaceColor;
    float frameNumMod = 1.0 - float((ofGetFrameNum() % (20))) / 20.0;
    float width;

    switch (colorScheme)
    {
    case ColorScheme::Z:
        surfaceColor.setHsb(ofMap(position.z, 0, 30, 0.392, 0.588, false), 1.0f, 1.0f, alpha);
        break;
    case ColorScheme::CIRCLE:
        width = ofMap(position.length(), 0, size * 3 * sqrt(2), 0.0f, 1.0f, true);
        surfaceColor.setHsb(ofWrap(width+frameNumMod, circleLower, circleUpper), 1.0f, 1.0f, alpha);
        break;
    }

    return surfaceColor;
}