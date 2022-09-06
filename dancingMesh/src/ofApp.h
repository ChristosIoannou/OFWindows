#pragma once

#include "ofMain.h"
#include "ofxFft.h"

//#define BUFFER_SIZE 512
//#define SR 44100

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    //--- setups ----
    void setupSoundStream();
    void setupFFT();
    void setupDancingMesh();
    void setupTetrahedron();

    //--- updates ---
    void updateFFTandAnalyse();
    void updateDancingMesh(float dt);
    void updateTetrahedron(float dt);

    //--- draw ---
    void drawInfo();
    void drawDancingMesh();
    void drawDancingMeshPoints();
    void drawDancingMeshLines();
    void drawTetrahedron();

    void audioIn(ofSoundBuffer& input);

    //--- fft ---
    void analyseFFT();
    ofxFft* fft;
    std::mutex soundMutex;
    float* audioInput;
    float* fftOutput;
    const int N = 32;		//Number of bands in spectrum
    std::vector<float> spectrum, soundSpectrum;      //Smoothed spectrum values
    int volumeMultiplier = 4;
    float bass, mids, highs, totals;
    int red, green, blue;
    float brightness;

    //--- soundStream ---
    ofSoundStream soundStream;
    int	sampleRate = 44100;
    int bufferSize = N*4;
    int inChan = 2;
    int outChan = 0;

    //--- dancingMesh ---
    std::vector<float> tx, ty;		//Offsets for Perlin noise calculation for points
    std::vector<ofPoint> p;			//Cloud's points positions
    const int n = 500;		//Number of cloud points
    float Rad = 500;		//Cloud raduis parameter
    float Vel = 0.1;		//Cloud points velocity parameter
    int bandRad = 1;		//Band index in spectrum, affecting Rad value
    int bandVel = 6;		//Band index in spectrum, affecting Vel value
    
    //--- tetrahedron ---
    ofMesh tetrahedron;
    std::vector<ofVec3f> tetTargetPoints;
    std::vector<ofVec3f> tetOldPoints;
    bool beat;

    //--- time ---
    float time0 = 0;		//Time value, used for dt computing
    
    //--- production ---
    ofEasyCam cam;

    //--- key booleans ---
    bool b_Info = false;
    bool b_dancingMesh = false;
    bool b_tetrahedron = true;

};