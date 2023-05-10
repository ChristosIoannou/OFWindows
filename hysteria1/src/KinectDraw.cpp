#include "KinectDraw.h"

void KinectDraw::setup() {

    depthImage.allocate(kinect.width, kinect.height);

    //for (int i = 0; i < 4; ++i)
    //{
    //    blobCentres.emplace_back(ofPoint(ofRandomWidth(), ofRandomHeight()));
    //}
}

void KinectDraw::update() {

    //if (kinect.isFrameNew())
    {
        // load grayscale depth image from the kinect source
        depthImage.setFromPixels(kinect.getDepthPixels());
        depthImage.threshold(nearThreshold, false);
        
        contourFinder.findContours(depthImage, 10, depthImage.width * depthImage.height / 4, 2, false);
    
        int numBlobs = contourFinder.nBlobs;

        //movePointsRandomly(blobCentres);

        for (const auto blob : contourFinder.blobs)
        {
            squares.emplace_back(
                blob.centroid,
                (int)ofRandom(360),
                (int)ofRandom(20, 50),
                ofGetElapsedTimef()
                );
        }
    }
}

void KinectDraw::draw() 
{
    //ofDisableDepthTest();
    //ofPushMatrix();
    //ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    float currentTime = ofGetElapsedTimef();

    for (int i = 0; i < squares.size(); i++) {
        float age = currentTime - squares[i].creationTime;

        if (age < maxAge) {
            float alpha = ofMap(age, 0.0, maxAge, 0.0, 255.0);
            float hue = ofMap(age, 0.0, maxAge, 0.0, 240.0);
            ofColor color = ofColor::fromHsb(flipHue ? 240 - hue : hue, 255.0, 255.0, flipHue ? 255 - alpha : alpha);

            ofPushMatrix();
            ofTranslate(squares[i].position.x, squares[i].position.y);
            ofRotateDeg(squares[i].rotation);
            ofScale(squares[i].size, squares[i].size);
            ofSetColor(color);
            ofNoFill();
            ofDrawRectangle(-0.5f, -0.5f, 1, 1);
            ofPopMatrix();

            // Update the square's properties
            squares[i].rotation += ofRandom(-2, 2);
            squares[i].size += ofRandom(-1, 1);
        }
        else {
            // Remove the square if its lifetime has expired
            squares.erase(squares.begin() + i);
            i--;
        }
    }

    //ofPopMatrix();
}


void KinectDraw::movePointsRandomly(vector<ofPoint>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        // Generate random angle between 0 and 2*PI radians
        float angle = ofMap(ofNoise(i, ofGetElapsedTimef()), 0, 1, 0, TWO_PI);

        // Generate random distance to move between 0 and maxMoveDist
        float distance = ofMap(ofNoise(i, ofGetElapsedTimef()), 0, 1, 0, 10.0);;

        // Calculate the change in x and y coordinates based on the angle and distance
        float dx = distance * cos(angle);
        float dy = distance * sin(angle);

        // Update the point's position
        points[i].x += dx;
        points[i].y += dy;

        // Keep the point within the screen bounds
        points[i].x = ofClamp(points[i].x, 0, ofGetWidth() - 1);
        points[i].y = ofClamp(points[i].y, 0, ofGetHeight() - 1);
    }
}