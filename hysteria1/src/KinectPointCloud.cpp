#include "KinectPointCloud.h"

void KinectPointCloud::setupKinectPointCloud() {
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    playMesh.setMode(OF_PRIMITIVE_POINTS);
}

void KinectPointCloud::updateKinectPointCloud() {
    if (b_explode) {
        usePlayMesh = true;
        explode();
    }
    else if (b_remerge) {
        usePlayMesh = true;
        remerge();
    }
    else {
        usePlayMesh = false;
        freezeKinectMesh = false;
    }
}

void KinectPointCloud::drawKinectPointCloud(ofEasyCam& cam) {
    ofSetColor(255, 255, 255);
    cam.begin();
    ofSetColor(ofColor::white);
    glPointSize(1);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();

    if (b_rotate) {
        cam.setTarget(centroid);
        ofRotateY(startOffsetAngle += 0.5);
        //ofRotateZ(ofGetFrameNum() * 0.5);
    }

    if (usePlayMesh) {
        playMesh.drawVertices();
    }
    else {
        kinectMesh.drawVertices();
    }
    ofDisableDepthTest();
    ofPopMatrix();

    //ofVec3f originVec(0, 0, 0);
    //ofSphere origin = ofSphere(originVec, 10);

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
        //b_explode = false;
        //b_remerge = true;
    }
}

void KinectPointCloud::rotateListener(bool& b_pcRotate) {
    b_rotate = b_pcRotate;
    if (b_rotate) {
        centroid = usePlayMesh ? playMesh.getCentroid() : kinectMesh.getCentroid();
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
       //kinectPC.playMesh.getVertices()[i] = kinectPC.playMesh.getVertices()[i] + kinectPC.playMesh.getVertices()[i] + pcDirections[i];
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
                    if (sparkle && (x % 20 == 0)) {
                        ofVec3f random(ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100));
                        kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y) + random);
                    }
                    kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
    }
}