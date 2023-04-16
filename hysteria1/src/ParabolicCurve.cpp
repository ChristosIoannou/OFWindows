#include "ParabolicCurve.h"

//--------------------------------------------------------
void ParabolicCurve::setup()
{
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

//--------------------------------------------------------
void ParabolicCurve::update()
{
    int numPoints = 20;

    if (regenerate)
    {
        axes.regenerateAxes(6, ofRandom(5, 40));
        regenerate = false;
    }

    if (flash)
    {
        fbo.begin();
        ofFill();
        ofSetColor(0, 0, 0, 25);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        drawFbo();
        fbo.end();
    }
    

}

//--------------------------------------------------------
void ParabolicCurve::draw()
{
    if (flash)
    {
        ofDisableDepthTest();
        ofSetColor(bezierColor);
        fbo.draw(0, 0);
    }
    else {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);  // Translate to the center of the screen
        axes.draw((Axes::LinesMode)linesMode.get());
        ofPopMatrix();
    }

}

//--------------------------------------------------------
void ParabolicCurve::drawFbo() {

    if (floor(ofRandom(spacing)) == 0) {
        bezierColor.setHsb(ofRandom(255), 255, 255, 255);
        ofSetColor(bezierColor);
        axes.regenerateAxes(6, ofRandom(10, 30));
    }
    ofNoFill();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 100);  // Translate to the center of the screen
    axes.draw((Axes::LinesMode)linesMode.get());
    ofPopMatrix();
    ofSetRectMode(OF_RECTMODE_CORNER);

}

//--------------------------------------------------------
void ParabolicCurve::resize() {
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
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
