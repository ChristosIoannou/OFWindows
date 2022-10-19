#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxKinect.h"
#include "KinectPointCloud.h"
#include "AudioSphere.h"
#include "ParticleRiver.h"
#include "KinectContour.h"
#include "Tunnel.h"
#include "DoController.h"
#include "SurfaceMesh.h"

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
    void setupKinect();
    void setupGui();
    void setupFFT();
    void setupBeatDetector();
    void setupAudioSphere();
    void setupFlashingText();
    void setupKinectPointCloud();
    void setupParticleRiver();
    void setupKinectContour();
    void setupTunnel();
    void setupSurfaceMesh();

    //--- update ---
    void updateFFTandAnalyse();
    void updateBeatDetector();
    void updateKinect();
    void updateAudioSphere();
    void updateFlashingText();
    void updateKinectPointCloud();
    void updateParticleRiver();
    void updateKinectContour();
    void updateTunnel();
    void updateSurfaceMesh();

    //--- draw ---
    void drawGui(ofEventArgs& args);
    void drawKinect();
    void drawAudioSphere();
    void drawFlashingText();
    void drawKinectPointCloud();
    void drawParticleRiver();
    void drawKinectContour();
    void drawTunnel();
    void drawSurfaceMesh();

    //--- DoController ---
    DoController doController;

    //--- soundStream ---
    ofSoundStream soundStream;
    const int sampleRate = FREQ_MAX;
    const int bufferSize = SPECTRAL_BANDS * 4;
    const int inChan = 2;
    const int outChan = 0;

    //--- fft ---
    void analyseFFT();
    ofxFft* fft;
    ofParameter<float> fftDecay;
    std::mutex soundMutex;
    float* audioInput;
    float* fftOutput;
    std::vector<float> spectrum, soundSpectrum;      //Smoothed spectrum values
    float bass, mids, highs, totals;
    int red, green, blue;
    float brightness;

    //--- beatDetector ---
    typedef std::deque<std::vector<float>> FFTHistoryContainer;
    static void fillAverageSpectrum(float* averageSpectrum, int numBands, const FFTHistoryContainer& fftHistory);
    static void fillVarianceSpectrum(float* varianceSpectrum, int numBands, const FFTHistoryContainer& fftHistory, const float* averageSpectrum);
    static float beatThreshold(float variance);
    int FFTHistoryMaxSize;
    std::vector<int> beatDetectorBandLimits;
    FFTHistoryContainer beatDetector;

    //--- audioSphere ---
    AudioSphere audioSphere;
    ofParameterGroup paramsAudioSphere;
    ofxPanel panelAudioSphere;
    ofParameter<bool> autoRotate;
    ofParameter<float> rotationSpeed;
    ofParameter<bool> rotateSin;
    ofParameter<float> posDecayRate; // = 0.996f; // 0.995f

    //--- ParticleRiver ---
    ParticleRiver particleRiver;
    ofParameterGroup paramsParticleRiver;
    ofxPanel panelParticleRiver;
    //ofParameter<bool> b_particleRiverDrawMap;

    //--- time ---
    float time0 = 0;		//Time value, used for dt computing

    //--- production ---
    ofEasyCam cam;

    //--- flashingText ---
    ofParameterGroup paramsFlashingText;
    ofxPanel panelFlashingText;
    ofParameter<std::string> inputMessage;
    ofParameter<float> markMax;            // max on time (s)
    ofParameter<float> spaceMax;         // max off time (s)
    ofParameter<int> framesLeft;    // (s)
    ofParameter<int> numFlashes;
    ofTrueTypeFont eastBorderFont;
    std::vector<int> flashFrames;
    int startingFrameNumber = 0;
    int flashIdx = 0;
    float colorRand = 0;
    int xshift, yshift;

    //--- kinect ---
    ofxKinect kinect;
    int angle;
    bool getKinectFrame = true;
    bool usePlayMesh = false;

    //--- KinectPointCloud ---
    KinectPointCloud kinectPointCloud = KinectPointCloud(kinect);
    ofParameterGroup paramsKinectPointCloud;
    ofxPanel panelKinectPointCloud;

    //--- KinectContour ---
    KinectContour kinectContour = KinectContour(kinect);
    ofParameterGroup paramsKinectContour;
    ofxPanel panelKinectContour;

    //--- Tunnel ---
    Tunnel tunnel;
    ofParameterGroup paramsTunnel;
    ofxPanel panelTunnel;
    //ofParameter<bool> b_tunnel;

    //--- SurfaceMesh ---
    SurfaceMesh surfaceMesh = SurfaceMesh(cam);
    ofParameterGroup paramsSurfaceMesh;
    ofxPanel panelSurfaceMesh;

    //--- GUI ---
    ofParameterGroup paramsSpectra;
    ofxPanel panelSpectra;
    ofParameter<bool> drawSpectrum;
    ofParameter<bool> drawBark;
    ofParameter<bool> drawSignal;
    
    ofParameterGroup paramsFFT;
    ofxPanel panelFFT;
    ofParameter<float> volumeMultiplier;

    ofParameterGroup paramsKinect;
    ofxPanel panelKinect;
    //ofParameter<bool> b_kinect;
    //ofParameter<bool> b_kinectPointCloud;

    

};
