#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"

class KinectDraw {

public:

    struct Square 
    {
        Square(ofPoint pos, float rot, int size_, float t)
            : position(pos)
            , rotation(rot)
            , size(size_)
            , creationTime(t)
        {}

        ofPoint position;
        float rotation;
        int size;
        float creationTime;
    };

    KinectDraw(ofxKinect& kinect_)
        : kinect(kinect_) {}

    void setup();
    void update();
    void draw();


    ofxKinect& kinect;
    ofxCvGrayscaleImage depthImage;
    ofxCvContourFinder contourFinder;
    
    ofParameter<int> nearThreshold;
    ofParameter<float> maxAge;
    ofParameter<bool> flipHue;

    vector<Square> squares;


    // rando stuff to be deleted
    //std::vector<ofPoint> blobCentres;
    void movePointsRandomly(vector<ofPoint>& points);
};