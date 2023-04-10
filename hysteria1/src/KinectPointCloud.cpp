#include "KinectPointCloud.h"

void KinectPointCloud::setup() {
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    playMesh.setMode(OF_PRIMITIVE_POINTS);

    // Listeners
    b_remerge.addListener(this, &KinectPointCloud::remergeListener);
    b_explode.addListener(this, &KinectPointCloud::explodeListener);
    //b_float.addListener(this, &KinectPointCloud::floatListener);
}

void KinectPointCloud::update() {
    if (kinect.isFrameNew())
        getNewFrame();

    if (b_remerge) {
        b_explode = false;
        usePlayMesh = true;
        remerge();
    }
    else if (b_explode) {
        usePlayMesh = true;
        explode();
    }
    else {
        usePlayMesh = false;
        freezeKinectMesh = false;
    }

}

void KinectPointCloud::draw(ofEasyCam& cam) {
    cam.begin();
    ofSetColor(255, 255, 255);
    glPointSize(1);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000);
    ofEnableDepthTest();

    if (!b_flash || (b_flash && floor(ofRandom(flashPeriod)) == 0)) 
    {
        if (usePlayMesh) {
            playMesh.drawVertices();
        }
        else {
            kinectMesh.drawVertices();
        }
    }

    ofDisableDepthTest();
    ofPopMatrix();
    cam.end();
}

//=============================================================

void KinectPointCloud::setKinectMesh(ofMesh mesh) {
	kinectMesh = mesh;
}

void KinectPointCloud::setPlayMesh(ofMesh mesh) {
	playMesh = mesh;
}

void KinectPointCloud::transferKinectMeshToPlayMesh() {
    //usePlayMesh = true;
    explodeDirections.clear();
    setPlayMesh(kinectMesh);
}

//============================================================

void KinectPointCloud::explodeListener(bool& b_explode_) {
    b_explode = b_explode_;
    if (b_explode)
        startExplode();
}

void KinectPointCloud::remergeListener(bool& b_pcRemerge) {
    b_remerge = b_pcRemerge;
    if (b_remerge) {
        freezeKinectMesh = true;
    }
}

void KinectPointCloud::startExplode(){
    transferKinectMeshToPlayMesh();
    for (int i = 0; i < playMesh.getNumVertices(); ++i) {
        ofVec3f direction(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
        explodeDirections.push_back(direction);
    }
}

void KinectPointCloud::explode() {
    for (int i = 0; i < playMesh.getNumVertices(); ++i) {
       playMesh.getVertices()[i] = playMesh.getVertices()[i] + explodeDirections[i];
    }
}

void KinectPointCloud::remerge() {
    playMesh.getVertices().resize(kinectMesh.getNumVertices());
    for (int i = 0; i < playMesh.getNumVertices(); ++i) {
        ofVec3f direction = kinectMesh.getVertices()[i] - playMesh.getVertices()[i];
        direction.normalize();
        playMesh.getVertices()[i] = playMesh.getVertices()[i] + direction;
    }
}

void KinectPointCloud::getNewFrame() {
    if (!freezeKinectMesh) {
        kinectMesh.clear();
        int step = 1;
        int shift = 0;
        getFullFrame();

    }
}

void KinectPointCloud::getFullFrame() {
    if (!freezeKinectMesh) {
        int w = kinect.width;
        int h = kinect.height;
        int step = 1;
        int shift = 0;
        for (int y = 0; y < h; y += step) {
            for (int x = (0 + shift); x < w; x += step) {
                if (kinect.getDistanceAt(x, y) > lowerThresh && kinect.getDistanceAt(x, y) < upperThresh) {

                    if (b_wave)
                    {
                        ofVec3f alter;
                        alter.x = ofNoise(ofMap(x, 0, w, 0, 5), ofMap(y, 0, h, 0, 5), ofGetElapsedTimef() * wave_freq) * wave_amplitude;
                        alter.y = ofNoise(ofMap(x, 0, w, 0, 5), ofMap(y, 0, h, 0, 5), ofGetElapsedTimef() * wave_freq) * wave_amplitude;
                        alter.z = ofNoise(ofMap(x, 0, w, 0, 5), ofMap(y, 0, h, 0, 5), ofGetElapsedTimef() * wave_freq) * wave_amplitude;

                        kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y) + alter);
                    }
                    else {
                        kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                    }
                    kinectMesh.addColor(calculateColor(kinect.getDistanceAt(x, y)));
                }
            }
        }
    }
}

ofFloatColor KinectPointCloud::calculateColor(float distance) {
    ofFloatColor surfaceColor;
    float frameNumMod = 1.0 - float((ofGetFrameNum() % (20))) / 20.0;
    float width;

    switch (colorScheme)
    {
    case KinectPointCloudColorScheme::RANDOM:
        surfaceColor.setHsb(ofRandom(1), 1.f, 1.f, 1.f);
        break;
    case KinectPointCloudColorScheme::DEPTH:
        surfaceColor.setHsb(ofMap(distance, 0, upperThresh, 0.f, 1.f, true), 1.0f, 1.0f, 0.8);
        break;
    case KinectPointCloudColorScheme::WHITE_POINTCLOUD:
        surfaceColor.set(1.f, 1.f, 1.f, 0.9);
        break;
    }

    return surfaceColor;
}
