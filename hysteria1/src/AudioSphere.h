#pragma once

#include "ofMain.h"

//bark mapping
#define BARK_MAX 19         // 25
#define SPECTRAL_BANDS 48   // 48 is good for Release, 32 for Debug
#define FREQ_MAX 5025.0     // 44100.0

struct pingPongBuffer {
public:
    void allocate(int _width, int _height, int _internalformat = GL_RGBA, float _dissipation = 1.0f) {
        // Allocate
        for (int i = 0; i < 2; i++) {
            FBOs[i].allocate(_width, _height, _internalformat);
        }

        // Clean
        clear();

        // Set everything to 0
        flag = 0;
        swap();
        flag = 0;
    }

    void swap() {
        src = &(FBOs[(flag) % 2]);
        dst = &(FBOs[++(flag) % 2]);
    }

    void clear() {
        for (int i = 0; i < 2; i++) {
            FBOs[i].begin();
            ofClear(0, 255);
            FBOs[i].end();
        }
    }

    ofFbo& operator[](int n) { return FBOs[n]; }

    ofFbo* src;       // Source       ->  Ping
    ofFbo* dst;       // Destination  ->  Pong
private:
    ofFbo   FBOs[2];    // Real addresses of ping/pong FBO«s
    int     flag;       // Integer for making a quick swap
};


class AudioSphere {

public:
    void setup();
    void update(std::vector<float>& spectrum, float posDecayRate);
    void draw(bool autoRotate, float rotationSpeed, bool rotateSin);

	void buildSphereMesh();
    int bark(float f);
	ofVboMesh sphereMesh; // the mesh of the sphere
    ofTexture eqTexture; //the eq curve texture we pass to the fbo
    ofPath eqPath; //we use this path to build the texture
    pingPongBuffer posBuffer;
    ofShader shader;
    ofShader posShader;

    float bins[BARK_MAX]; //where we sum up our spectral info
    int barkmap[SPECTRAL_BANDS]; //if i is the index of spectrum band, barkmap[i] gives which bin it goes to.
    int sphereRadius = 300; //sphere radius
    int sphereResolution = 250; //sphere resolution
    int fboResolution; //fbo resolution, will be sphere resolution
    float axisLocation; //which part of the axis to update
    float startOffsetAngle = 90.0f; //start offset for xaxis
    float angleIncrement;
};