#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);
    ofSetFrameRate(-1);

    // N = 32
    // bufferSize = 128
    setupSoundStream();
    setupFFT();
    setupDancingMesh();

}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();

    //Update particles using spectrum values
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time; //Store the current time

    updateDancingMesh(dt);

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(ofColor::black);	//Set up the background
    ofEnableAlphaBlending();


    if (showInfo) {
        drawInfo();
    }

    if (dancingMesh) {
        drawDancingMesh();
    }

}



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
    case 'g':
        showInfo = !showInfo;
        break;
    case 'd':
        dancingMesh = !dancingMesh;
        break;
    case '=':
        volumeMultiplier++;
        break;
    case '-':
        volumeMultiplier--;
        break;
    case 'f':
        ofToggleFullscreen();
        break;
    case 'w':
        if (bandRad < N)
            bandRad++;
        break;
    case 'q':
        if (bandRad > 0)
            bandRad--;
        break;
    case 's':
        if (bandVel < N)
            bandVel++;
        break;
    case 'a':
        if (bandVel > 0)
            bandVel--;
        break;
    default:
        break;
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
    
    audioInput = input.getBuffer().data();
    fft->setSignal(audioInput);
    memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * N);

    for (int i = 0; i < N; i++) {
        fftOutput[i] = fftOutput[i] * (volumeMultiplier / 4);
    }

    ////We should not release memory of val,
    ////because it is managed by sound engine
    ////Update our smoothed spectrum,
    ////by slowly decreasing its values and getting maximum with val
    ////So we will have slowly falling peaks in spectrum
    soundMutex.lock();
    for (int i = 0; i < N; i++) {
        soundSpectrum[i] *= 0.99;	//Slow decreasing
        soundSpectrum[i] = max(soundSpectrum[i], fftOutput[i]);
    }
    soundMutex.unlock();

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
void ofApp::setupSoundStream() {
    ofSoundStreamSettings settings;

    //soundStream.printDeviceList();
    auto devices = soundStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);

    for (int i = 0; i < devices.size(); ++i) {
        std::cout << "[MS_WASAPI : " << devices[i].deviceID << "] | Name: " << devices[i].name <<
            " [in: " << devices[i].inputChannels <<
            " , out: " << devices[i].outputChannels << "]" << std::endl;
    }

    int deviceId;
    std::cout << "Choose input device index for MS_WASAPI: " << std::endl;
    std::cin >> deviceId;
    settings.setInDevice(devices[deviceId]);
    std::cout << "Device selected: " << devices[deviceId].name << std::endl;

    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::setupFFT() {
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
    audioInput = new float[bufferSize];
    fftOutput = new float[N];
    spectrum.resize(N, 0.0f);
    soundSpectrum.resize(N, 0.0f);
}

//--------------------------------------------------------------
void ofApp::setupDancingMesh() {
    p.resize(n);

    for (int i = 0; i < n; ++i) {
        tx.push_back(ofRandom(0, 1000));
        ty.push_back(ofRandom(0, 1000));
    }
}

//--------------------------------------------------------------
void ofApp::updateFFTandAnalyse() {
    soundMutex.lock();
    spectrum = soundSpectrum;
    soundMutex.unlock();

    analyseFFT();
}

//--------------------------------------------------------------
void ofApp::updateDancingMesh(float dt) {
    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    Rad = ofMap(spectrum[bandRad], 1, 3, 500, 1200, true);
    Vel = ofMap(spectrum[bandVel], 0, 0.1, 0.05, 0.5);
    //Update particles positions
    for (int j = 0; j < n; j++) {
        tx[j] += Vel * dt;	//move offset
        ty[j] += Vel * dt;	//move offset
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        p[j].x = ofSignedNoise(tx[j]) * Rad;
        p[j].y = ofSignedNoise(ty[j]) * Rad;
    }
}

//--------------------------------------------------------------
void ofApp::analyseFFT() {

    //Calculate color based on range
    std::vector<float>::iterator rg_it = std::next(spectrum.begin(), 3);
    std::vector<float>::iterator gb_it = std::next(spectrum.begin(), 20);

    bass = std::accumulate(spectrum.begin(), rg_it, 0.0f);
    mids = std::accumulate(rg_it, gb_it, 0.0f);
    highs = std::accumulate(gb_it, spectrum.end(), 0.0f);
    totals = std::accumulate(spectrum.begin(), spectrum.end(), 0.0f);

    red = static_cast<int>(std::min(ofMap(bass, 0, 6, 0, 255), 255.0f));
    green = static_cast<int>(std::min(ofMap(mids, 0, 6, 0, 255), 255.0f));
    blue = static_cast<int>(std::min(ofMap(highs, 0, 6, 0, 255), 255.0f));
    brightness = std::min(ofMap(totals, 0, 10, 100, 255), 255.0f);
}

//--------------------------------------------------------------
void ofApp::drawDancingMesh() {
    ofDisableDepthTest();
    ofPushMatrix();         // Move center of coordinate system to the screen center
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    drawDancingMeshPoints();
    drawDancingMeshLines();
    ofPopMatrix();          // Restore coordinate system
    ofEnableDepthTest();
}

//--------------------------------------------------------------
void ofApp::drawDancingMeshPoints() {
    ofEnableAlphaBlending();
    ofFill();
    for (int i = 0; i < n; ++i) {
        float hue = ofMap(static_cast<float>(i), 0.0, static_cast<float>(n), 170.0f, 255.0f);
        ofColor drawColor;
        drawColor.setHsb(hue, 255, brightness, 120);
        ofSetColor(drawColor); // higher alpha is more opaque
        ofDrawCircle(p[i], 2);
    }
}

//--------------------------------------------------------------
void ofApp::drawDancingMeshLines() {
    float dist = 60;	//Threshold parameter of distance
    for (int j = 0; j < n; ++j) {
        for (int k = j + 1; k < n; ++k) {
            if (ofDist(p[j].x, p[j].y, p[k].x, p[k].y)
                < dist) {
                float hue = ofMap(static_cast<float>(j), 0.0, static_cast<float>(n), 170.0f, 255.0f);
                ofColor drawColor;
                drawColor.setHsb(hue, 255, brightness, 120);
                ofSetColor(drawColor); // higher alpha is more opaque
                ofDrawLine(p[j], p[k]);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawInfo() {
    ofDisableDepthTest();
    //Draw background rect for spectrum
    ofSetColor(ofColor::black);
    ofFill();
    ofDrawRectangle(10, 700, N * 6, -100);
    //Draw spectrum
    //ofSetColor( ofColor::ghostWhite );
    for (int i = 0; i < N; i++) {
        //Draw bandRad and bandVel by black color,
        //and other by gray color
        if (i == bandRad) {
            ofSetColor(ofColor::red); //Black color
        }
        else if (i == bandVel) {
            ofSetColor(ofColor::green);
        }
        else {
            ofSetColor(ofColor::lightGoldenRodYellow); //Gray color
        }
        ofDrawRectangle(10 + i * 5, 700, 3, -spectrum[i] * 100);
    }

    ofDrawBitmapString("screen      | fps: " + ofToString(ofGetFrameRate()), 10, 10);
    ofDrawBitmapString("soundStream | bufferSize: " + ofToString(soundStream.getBufferSize()) + ", sampleRate: " + ofToString(soundStream.getSampleRate()), 10, 20);
    ofDrawBitmapString("fft         | binSize: " + ofToString(fft->getBinSize()) + ", sampleSize: " + ofToString(fft->getSignalSize()), 10, 30);
    ofDrawBitmapString("spectrum    | numofbins: " + ofToString(spectrum.size()) + ", N: " + ofToString(N), 10, 40);
    ofEnableDepthTest();
}
