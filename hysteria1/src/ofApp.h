#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxKinect.h"
#include "KinectPointCloud.h"
#include "AudioSphere.h"

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

    //--- update ---
    void updateFFTandAnalyse();
    void updateBeatDetector();
    void updateKinect();
    void updateAudioSphere();
    void updateFlashingText();
    void updateKinectPointCloud();

    //--- draw ---
    void drawGui(ofEventArgs& args);
    void drawKinect();
    void drawAudioSphere();
    void drawFlashingText();
    void drawKinectPointCloud();

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

    //--- time ---
    float time0 = 0;		//Time value, used for dt computing

    //--- production ---
    ofEasyCam cam;

    //--- flashingText ---
    ofParameterGroup paramsFlashingText;
    ofxPanel panelFlashingText;
    ofParameter<std::string> inputMessage;
    ofParameter<bool> b_flashingText;
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
    KinectPointCloud kinectPC;// (OF_PRIMITIVE_POINTS);
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCvContourFinder contourFinder;
    int angle;
    bool getKinectFrame = true;
    bool usePlayMesh = false;

    //--- KinectPointCloud ---
    void remergeListener(bool& b_pcRemerge);
    ofParameterGroup paramsPcExplode;
    ofxPanel panelPcExplode;
    ofParameter<bool> b_pcRemerge;
    ofParameter<bool> b_pcExplode;
    ofParameter<bool> b_pcSparkle;
    ofParameter<bool> b_pcRotate;

    //--- GUI ---
    ofParameterGroup paramsSpectra;
    ofxPanel panelSpectra;
    ofParameter<bool> drawSpectrum;
    ofParameter<bool> drawBark;
    ofParameter<bool> drawSignal;
    
    ofParameterGroup paramsFFT;
    ofxPanel panelFFT;
    ofParameter<float> volumeMultiplier;

    ofParameterGroup paramsAudioSphere;
    ofxPanel panelAudioSphere;
    ofParameter<bool> b_audioSphere;
    ofParameter<bool> autoRotate;
    ofParameter<float> rotationSpeed;
    ofParameter<bool> rotateSin;
    ofParameter<float> posDecayRate; // = 0.996f; // 0.995f

    ofParameterGroup paramsKinect;
    ofxPanel panelKinect;
    ofParameter<bool> b_kinect;
    ofParameter<bool> bDrawPointCloud;
    ofParameter<bool> bThreshWithOpenCV;
    ofParameter<int> nearThreshold;
    ofParameter<int> farThreshold;
};
