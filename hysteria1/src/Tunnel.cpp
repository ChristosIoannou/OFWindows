#include "Tunnel.h"

void Tunnel::setup() {
    ofFboSettings s;
    s.width = ofGetWidth();
    s.height = ofGetWidth();
    s.internalformat = GL_RGBA32F_ARB;
    s.useDepth = true;
    fbo.allocate(s);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
}

void Tunnel::update() {
    fbo.begin();
    ofFill();
    ofSetColor(0, 0, 0, 25);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    drawFbo();
    fbo.end();
}

void Tunnel::resize() {
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    ofFboSettings s;
    s.width = ofGetWidth();
    s.height = ofGetWidth();
    s.internalformat = GL_RGBA32F_ARB;
    s.useDepth = true;
    fbo.allocate(s);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
}

void Tunnel::drawFbo() {
    ofSetRectMode(OF_RECTMODE_CENTER);
    //ofSetColor(ofColor::aquamarine);
    if (floor(ofRandom(20)) == 0) {
        tunnelColor.setHsb(ofRandom(255), 255, 255, 255);
        ofSetColor(tunnelColor);
    }
    ofNoFill();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 100);  // Translate to the center of the screen
    for (int i = 0; i < 100; i++) {
        ofScale(1.1, 1.1, -1.1);
        float time = ofGetElapsedTimef();
        float timeScale = 0.5;
        float noise = ofSignedNoise(time * timeScale) * 30.0;
        ofRotate(noise);
        ofDrawRectangle(0, 0, 50, 50);
    }
    ofPopMatrix();
    ofSetRectMode(OF_RECTMODE_CORNER);

}

void Tunnel::draw() {

    //ofSetColor(255, 255, 255);
    ofDisableDepthTest();
    ofSetColor(tunnelColor);
    fbo.draw(0, 0);

}