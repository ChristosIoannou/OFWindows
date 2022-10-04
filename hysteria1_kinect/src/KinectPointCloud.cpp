#include "KinectPointCloud.h"

void KinectPointCloud::setupKinectPointCloud() {
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    playMesh.setMode(OF_PRIMITIVE_POINTS);
    freezeKinectMesh = false;
}

void KinectPointCloud::updateKinectPointCloud() {
    if (b_explode) {
        usePlayMesh = true;
        explode();
    }
    else {
        //usePlayMesh = false;
        remergeExplode();
    }
}

void KinectPointCloud::drawKinectPointCloud() {
    if (usePlayMesh)
        playMesh.drawVertices();
    else
        kinectMesh.drawVertices();
}

//=============================================================

void KinectPointCloud::setKinectMesh(ofMesh mesh) {
	kinectMesh = mesh;
}

void KinectPointCloud::setPlayMesh(ofMesh mesh) {
	playMesh = mesh;
}

void KinectPointCloud::transferKinectMeshToPlayMesh() {
    usePlayMesh = true;
    if (usePlayMesh) {
        explodeDirections.clear();
        setPlayMesh(kinectMesh);
    }
}

//============================================================

void KinectPointCloud::explodeListener(bool& b_explode_) {
    b_explode = b_explode_;
    if (b_explode_)
        startExplode();
    else
        int j = 0;
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
       //kinectPC.playMesh.getVertices()[i] = kinectPC.playMesh.getVertices()[i] + kinectPC.playMesh.getVertices()[i] + pcDirections[i];
       playMesh.getVertices()[i] = playMesh.getVertices()[i] + explodeDirections[i];
    }
}

void KinectPointCloud::remergeExplode() {
    //freezeKinectMesh = true;
    playMesh.getVertices().resize(kinectMesh.getNumVertices());
    for (int i = 0; i < playMesh.getNumVertices(); ++i) {
        ofVec3f direction = kinectMesh.getVertices()[i] - playMesh.getVertices()[i];
        //direction.normalize();
        playMesh.getVertices()[i] = playMesh.getVertices()[i] + direction*0.01;
    }
}

void KinectPointCloud::getNewFrame(ofxKinect& kinect) {
    if (!freezeKinectMesh) {
        kinectMesh.clear();
        int w = kinect.width;
        int h = kinect.height;
        int step = 1;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                if (kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1500) {
                    //mesh.addColor(kinect.getColorAt(x, y));
                    kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
    }
}