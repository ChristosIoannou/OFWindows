#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

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


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioIn(ofSoundBuffer& input);

    //--- setup ----
    void setupSoundStream();
    void setupGui();
    void setupFFT();
    void setupDancingMesh();
    void setupAudioSphere();

    //--- update ---
    void updateFFTandAnalyse();
    void updateDancingMesh(float dt);
    void updateAudioSphere();

    //--- draw ---
    void drawGui(ofEventArgs& args);
    void drawAudioSphere();
    void drawDancingMesh();
    void drawDancingMeshPoints();
    void drawDancingMeshLines();

    //--- soundStream ---
    ofSoundStream soundStream;
    const int sampleRate = FREQ_MAX;
    const int bufferSize = SPECTRAL_BANDS * 4;
    const int inChan = 2;
    const int outChan = 0;

    //--- fft ---
    void analyseFFT();
    ofxFft* fft;
    std::mutex soundMutex;
    float* audioInput;
    float* fftOutput;
    std::vector<float> spectrum, soundSpectrum;      //Smoothed spectrum values
    float bass, mids, highs, totals;
    int red, green, blue;
    float brightness;

    //--- dancingMesh ---
    std::vector<float> tx, ty;		//Offsets for Perlin noise calculation for points
    std::vector<ofPoint> p;			//Cloud's points positions
    const int n = 500;		//Number of cloud points
    float Rad = 500;		//Cloud raduis parameter
    float Vel = 0.1;		//Cloud points velocity parameter

    //--- audioSphere ---
    void buildSphereMesh(int radius, int sphereResolution, ofMesh& sphereMesh);
    ofTexture eqTexture; //the eq curve texture we pass to the fbo
    ofPath eqPath; //we use this path to build the texture
    ofVboMesh vm; //the mesh of the sphere
    ofShader shader;
    ofShader dispShader;
    ofShader posShader;
    pingPongBuffer posBuffer;
    float bins[BARK_MAX]; //where we sum up our spectral info
    int barkmap[SPECTRAL_BANDS]; //if i is the index of spectrum band, barkmap[i] gives which bin it goes to.
    int sphereRadius = 300; //sphere radius
    int sphereResolution = 250; //sphere resolution
    int fboResolution; //fbo resolution, will be sphere resolution
    float axisLocation; //which part of the axis to update
    float posDecayRate = 0.994f; // 0.995f
    float startOffsetAngle = 90.0f; //start offset for xaxis
    float angleIncrement;

    //--- time ---
    float time0 = 0;		//Time value, used for dt computing

    //--- production ---
    ofEasyCam cam;

    //--- key booleans ---

    //--- GUI ---
    ofParameterGroup paramsSpectra;
    ofxPanel panelSpectra;
    ofParameter<bool> drawSpectrum;
    ofParameter<bool> drawBark;
    ofParameter<bool> drawSignal;
    
    ofParameterGroup paramsFFT;
    ofxPanel panelFFT;
    ofParameter<float> volumeMultiplier;

    ofParameterGroup paramsDancingMesh;
    ofxPanel panelDancingMesh;
    ofParameter<bool> b_dancingMesh;
    ofParameter<int> bandRad;		//Band index in spectrum, affecting Rad value
    ofParameter<int> bandVel;		//Band index in spectrum, affecting Vel value

    ofParameterGroup paramsAudioSphere;
    ofxPanel panelAudioSphere;
    ofParameter<bool> b_audioSphere;
    ofParameter<bool> autoRotate;
    ofParameter<float> rotationSpeed;
    ofParameter<bool> rotateSin;


    //ofParameter<ofColor> color;
    ofParameter<bool> b_Info = false;
    //ofParameter<bool> b_dancingMesh = false;
    //ofParameter<bool> b_audioSphere = true;
    //ofParameter<bool> autoRotate;

    

};
