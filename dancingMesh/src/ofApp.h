#pragma once

#include "ofMain.h"
#include "ofxFft.h"

#define BUFFER_SIZE 512
#define SR 44100

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

    void audioIn(ofSoundBuffer& input);

    ofxFft* fft;
    float* audioInput;
    float* fftOutput;

    void analyseFFT();

    ofSoundStream soundStream;

    int	sampleRate;
    int bufferSize;
    int inChan;
    int outChan;

    int volumeMultiplier = 4;

    bool play;
    const int N = 256;		//Number of bands in spectrum
    std::vector<float> spectrum;      //Smoothed spectrum values
    float Rad = 500;		//Cloud raduis parameter
    float Vel = 0.1;		//Cloud points velocity parameter
    int bandRad = 1;		//Band index in spectrum, affecting Rad value
    int bandVel = 100;		//Band index in spectrum, affecting Vel value
    const int n = 500;		//Number of cloud points
    std::vector<float> tx, ty;		//Offsets for Perlin noise calculation for points
    std::vector<ofPoint> p;			//Cloud's points positions
    float time0 = 0;		//Time value, used for dt computing
    bool show_spectrum;
    float bass, mids, highs, totals;
    int red, green, blue;
    float brightness;
};