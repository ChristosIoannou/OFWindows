#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"

class KinectContour {

public:
    KinectContour(ofxKinect& kinect_)
        : kinect(kinect_) {}
        
    void setup();
    void update();
    void draw(float bass);

    void concentricContours(float bass);

    ofxKinect& kinect;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCvContourFinder contourFinder;

    ofParameter<int> nearThreshold;
    ofParameter<int> farThreshold;
    ofParameter<bool> continuousConcentric;
    ofParameter<int> nContours;
    ofParameter<float> sizeRatio;
};