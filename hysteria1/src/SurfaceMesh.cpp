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
            if (rangeDependent) {
                alpha = 0.375;
                float bassContrib = ofNoise(ofMap(x, 0, size * 3, 0, 5), ofMap(y, 0, size * 3, 0, 5), ofGetElapsedTimef()) * bass / 0.5;
                float midsContrib = ofNoise(ofMap(x, 0, size * 3, 0, 12), ofMap(y, 0, size * 3, 0, 12), ofGetElapsedTimef()) * mids / 1.5;
                float highsContrib = ofNoise(ofMap(x, 0, size * 3, 0, 30), ofMap(y, 0, size * 3, 0, 30), ofGetElapsedTimef()) * highs / 3.0;
                position.z = (bassContrib + midsContrib + highsContrib) * 20;
            }
            else {
                alpha = 0.3;
                if (usePerlin)
                    position.z = (ofMap(ofSignedNoise(count, ofGetElapsedTimef()), -1, 1, 0, bass * 30));
                else
                    position.z = (ofMap(ofNoise(count, ofGetElapsedTimef()), 0, 1, 0, bass * 30));
            }
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
        ofTranslate(0, 0, (abs(cos(ofGetElapsedTimef()) * 0.07)) * 3000);
    if (rotateZ)
        ofRotateZ(ofSignedNoise(ofGetElapsedTimef() * 0.07) * 720.0);
    if (rotateX)
        ofRotateX(ofSignedNoise(ofGetElapsedTimef() * 0.07) * 720.0);
    if (rotateY)
        ofRotateY(ofSignedNoise(ofGetElapsedTimef() * 0.07) * 720.0);

    if (wireframe) {
        glPointSize(1);
        mesh.drawWireframe();
    }
    else {
        glPointSize(1.7);
        mesh.drawVertices();
    }
    
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
    case ColorScheme::WHITE:
        surfaceColor.set(1.f, 1.f, 1.f, alpha);
    }

    return surfaceColor;
}