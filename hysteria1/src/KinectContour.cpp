#include "KinectContour.h"

void KinectContour::setup() {

    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);

}

void KinectContour::update() {
    if (kinect.isFrameNew()) {

        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);

        // update the cv images
        grayImage.flagImageChanged();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 10, (kinect.width * kinect.height) / 2, 20, false);
    }
}

void KinectContour::draw(float bass) {

    if (continuousConcentric)
        concentricContours(bass);

}

void KinectContour::concentricContours(float bass) {
    //contourFinder.draw(-ofGetWidth() / 2, -ofGetHeight() / 2, ofGetWidth(), ofGetHeight(), false);
    sizeRatio = min(bass, (float)pow(1.4f, 1/nContours));
    for (int i = 0; i < nContours; ++i) {
        if (((ofGetFrameNum() / 5) + i) % (nContours + 1) == 0)
            continue;

        // somehow introduce a delay here maybe ?
        float width = pow(sizeRatio, float(i)) * ofGetWidth();
        float height = pow(sizeRatio, float(i)) * ofGetHeight();
        ofColor contourColor;
        contourColor.setHsb(ofRandom(255), 255, 255, 255 * pow(sizeRatio, float(i)));
        ofSetColor(contourColor);
        contourFinder.draw(-width / 2, (ofGetHeight() / 2 - height), width, height, false);
    }
}