#include "KinectPointCloud.h"

void KinectPointCloud::setup() {
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    playMesh.setMode(OF_PRIMITIVE_POINTS);

    // Listeners
    b_remerge.addListener(this, &KinectPointCloud::remergeListener);
    b_explode.addListener(this, &KinectPointCloud::explodeListener);
    b_rotate.addListener(this, &KinectPointCloud::rotateListener);

    ofVec3f origin(0, 0, 0);
    ofVec3f x(1, 0, 0);
    ofVec3f y(0, 1, 0);
    ofVec3f z(0, 0, 1);
    ofSetColor(ofColor::red);
    ofDrawArrow(origin, x);
    ofSetColor(ofColor::green);
    ofDrawArrow(origin, y);
    ofSetColor(ofColor::blue);
    ofDrawArrow(origin, x);

    ofVec3f downwards(y);
    ofSetSmoothLighting(true);
    light.setDiffuseColor(ofColor(225.f, 255.f, 255.f));
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    light.setSpotlight();
    light.setSpotlightCutOff(50);
    light.setSpotlightCutOff(24);
    //light.setOrientation({ 90, 0, 0 });
    light.setPosition({ 0, 1000, 0 });
    material.setShininess(120);
    material.setSpecularColor(ofColor(255, 255, 255, 255));
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

    light.setOrientation({ lightOriX, lightOriY, lightOriZ });
    light.setPosition({ lightPosX, lightPosY, lightPosZ });
}

void KinectPointCloud::draw(ofEasyCam& cam) {
    ofSetColor(255, 255, 255);
    ofEnableLighting();
    cam.begin();
    light.enable();
    ofSetColor(ofColor::white);
    glPointSize(1);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(-1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();


    if (b_rotate) {
        cam.setTarget(centroid);
        ofRotateY(startOffsetAngle += 0.5);
    }

    if (!b_trapped) {
        if (usePlayMesh) {
            playMesh.drawVertices();
        }
        else {
            kinectMesh.drawVertices();
        }
    }
    else {
        drawTrapped();
    }
    ofDisableDepthTest();
    ofPopMatrix();

    ofDisableLighting();
    ofSetColor(light.getDiffuseColor());
    light.draw();
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

        if (!b_trapped) {
            int step;
            float shift;
            if (b_shimmer) {
                step = 2;
                shift = sin(ofGetFrameNum() / 4) >= 0.0 ? 1.0f : 0.0f;
            }
            else {
                step = 1;
                shift = 1;
            }
            getFullFrame(step, shift);
        }
        else {
            getReducedFrame();
        }
    }
}

void KinectPointCloud::getFullFrame(int step, int shift) {
    if (!freezeKinectMesh) {
        int w = kinect.width;
        int h = kinect.height;
        for (int y = 0; y < h; y += step) {
            for (int x = (0 + shift); x < w; x += step) {
                if (kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1500) {
                    kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
    }
}

void KinectPointCloud::getReducedFrame() {
    if (!freezeKinectMesh) {
        int w = kinect.width;
        int h = kinect.height;
        int step = 2;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += floor(ofRandom(8))) {
                if (kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1500) {
                    kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
    }
}

void KinectPointCloud::drawTrapped() {

    material.begin();

    ofSetColor(ofColor::lightGray);
    for (int i = 0; i < kinectMesh.getNumVertices(); ++i) {

        //ofSetColor(ofColor::lightGray);
        ofDrawSphere(kinectMesh.getVertex(i), 3.5);
        if (floor(ofRandom(20)) == 0) {
           // ofSetColor(ofColor::darkGray, 20);
            ofDrawLine(kinectMesh.getVertex(i), kinectMesh.getVertex(ofRandom(kinectMesh.getNumVertices())));
        }
    }

    material.end();
}