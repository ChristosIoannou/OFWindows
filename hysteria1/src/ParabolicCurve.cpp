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

    startTime = ofGetElapsedTimef();
    regenerate.addListener(this, &ParabolicCurve::regenerateListener);
    index = 0;
}

//--------------------------------------------------------
void ParabolicCurve::update()
{
    int numPoints = 20;

    
    //if (regenerate)
    //{
    //    axes.regenerateAxes(6, ofRandom(5, 40));
    //    regenerate = false;
    //}
    fbo.begin();

    switch ((PlayMode)playMode.get())
    {
    case PlayMode::FLASH:
        ofFill();
        ofSetColor(0, 0, 0, 25);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        drawFboFlash();
        break;

    case PlayMode::BUILD:
        drawFboBuild(numPoints);
        break;
    }
   

    fbo.end();

}

//--------------------------------------------------------
void ParabolicCurve::draw()
{
    ofDisableDepthTest();
    ofSetColor(bezierColor);
    fbo.draw(0, 0);
}

//--------------------------------------------------------
void ParabolicCurve::drawFboFlash() {

    if (floor(ofRandom(spacing)) == 0) {
        bezierColor.setHsb(ofRandom(255), 255, 255, 255);
        ofSetColor(bezierColor);
        axes.regenerateAxes(6, ofRandom(10, 30));
    }
    ofNoFill();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 100);  // Translate to the center of the screen
    axes.drawAll(linesMode);
    ofPopMatrix();
    ofSetRectMode(OF_RECTMODE_CORNER);

}

void ParabolicCurve::drawFboBuild(int numPoints)
{
    ofSetColor(255);
    ofNoFill();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 100);  // Translate to the center of the screen
    axes.xaxis.draw();
    axes.yaxis.draw();
    
    //int index = ofGetFrameNum() % numPoints;

    std::vector<ofPolyline> crosshatchLines;
    crosshatchLines.insert(crosshatchLines.end(), axes.crosshatch[0].begin(), axes.crosshatch[0].end());
    crosshatchLines.insert(crosshatchLines.end(), axes.crosshatch[1].begin(), axes.crosshatch[1].end());

    // Calculate the elapsed time since the start
    float elapsedTime = ofGetElapsedTimef() - startTime;

    // Calculate the index of the polyline to show based on the frequency
    int newIndex = int(elapsedTime * 16.0);

    // If the index has changed, show the new polyline
    if (newIndex != index) {
        index = newIndex;
        if (index < crosshatchLines.size()) {
            ofPolyline currentPolyline = crosshatchLines[index];
            // Draw the new polyline onto the off-screen buffer
            currentPolyline.draw();
        }
    }
        
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
void ParabolicCurve::regenerateListener(bool& regenerate)
{
    if (regenerate)
    {
        axes.regenerateAxes(6, ofRandom(5, 40));
        startTime = ofGetElapsedTimef();
        fbo.begin();
        ofClear(255, 255, 255, 0);
        fbo.end();
        regenerate = false;
    }
}

//--------------------------------------------------------
void ParabolicCurve::playModeListener(int& playMode)
{
    axes.regenerateAxes(6, ofRandom(5, 40));
    startTime = ofGetElapsedTimef();
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
}
 
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
