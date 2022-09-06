#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);
    ofSetFrameRate(-1);

    // N = 32, bufferSize = 128
    setupSoundStream();
    setupFFT();
    setupDancingMesh();
    setupTetrahedron();
}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();
    beatDetector.update(ofGetElapsedTimeMillis());

    //Update particles using spectrum values
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time; //Store the current time

    updateDancingMesh(dt);

    //float kick = beatDetector.kick();
    if (beatDetector.isKick(beatSensitivity / 10.0f)) {
        beat = true;
    }
    updateTetrahedron(dt);

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(ofColor::black);	//Set up the background
    ofEnableAlphaBlending();

    if (b_Info) {
        drawInfo();
    }

    ofPushMatrix();         // Move center of coordinate system to the screen center
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    if (b_dancingMesh) {
        drawDancingMesh();
    }

    if (b_tetrahedron) {
        drawTetrahedron();
    }

    ofPopMatrix();          // Restore coordinate system
}

void ofApp::drawTetrahedron() {
    ofNoFill();
    tetrahedron.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
    case 'g':
        b_Info = !b_Info;
        break;
    case 'd':
        b_dancingMesh = !b_dancingMesh;
        break;
    case 't':
        b_tetrahedron = !b_tetrahedron;
        break;
    case '=':
        volumeMultiplier++;
        break;
    case '-':
        volumeMultiplier--;
        break;
    case '0':
        beatSensitivity++;
        break;
    case '9':
        beatSensitivity--;
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
    case 'r':
        beatDetector.disableBeatDetect();
        beatDetector.enableBeatDetect();
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
        fftOutput[i] = fftOutput[i] * (volumeMultiplier / 4.0f);
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

    beatDetector.audioReceived(audioInput, bufferSize, inChan);

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
    beat = true;
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
void ofApp::setupTetrahedron() {

    tetrahedron.setMode(OF_PRIMITIVE_LINES);
    tetrahedron.enableColors();
    tetTargetPoints.resize(4);
    tetOldPoints.resize(4);

    for (int a = 0; a < 4; ++a) {
        ofVec3f temp(ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius));
        ofVec3f targettemp(ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius));
        ofFloatColor drawColor;
        float h = 158.0 / 255.0;
        float s = 240.0 / 255.0;
        float b = 128.0 / 255.0;
        drawColor.setHex(0x51DCAA, 1.0f); //158,63,86
        tetrahedron.addVertex(temp);
        tetrahedron.addColor(drawColor);
        tetTargetPoints[a] = targettemp;
        for (int b = a + 1; b < 5; ++b) {
            tetrahedron.addIndex(a);
            tetrahedron.addIndex(b % 4);
        }
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
void ofApp::updateTetrahedron(float dt) {
    if (beat) {
        for (int i = 0; i < 4; ++i) {
            tetOldPoints[i] = tetrahedron.getVertex(i);
            ofVec3f temp(ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius), ofRandom(-tetRadius, tetRadius));
            tetTargetPoints[i] = temp;
        }
        beat = false;
    }

    for (int i = 0; i < 4; ++i) {
        ofVec3f moveDirection = tetTargetPoints[i] - tetOldPoints[i];
        moveDirection.normalize();
        moveDirection *= moveVelocity;
        ofVec3f newPosition = tetrahedron.getVertex(i) + moveDirection * dt;
        tetrahedron.setVertex(i, newPosition);
    }

    //beat = passedTarget();
}

//--------------------------------------------------------------
bool ofApp::passedTarget() {
    ofVec3f moveDirection = tetTargetPoints[0] - tetOldPoints[0];
    if (moveDirection.dot((tetTargetPoints[0] - tetrahedron.getVertex(0))) < 0)
        return true;
    else
        return false;
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
    drawDancingMeshPoints();
    drawDancingMeshLines();
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
    ofDrawBitmapString("soundStream | bufferSize: " + ofToString(soundStream.getBufferSize()) + ", sampleRate: " + ofToString(soundStream.getSampleRate()), 10, 22);
    ofDrawBitmapString("fft         | binSize: " + ofToString(fft->getBinSize()) + ", volume: " + ofToString(volumeMultiplier/4.0f), 10, 34);
    ofDrawBitmapString("spectrum    | band[0] " + ofToString(spectrum[0]) + ", band[1]: " + ofToString(spectrum[1]), 10, 46);
    ofDrawBitmapString("beat        | beat: " + ofToString(beatDetector.isKick()) + ", snare: " + ofToString(beatDetector.isSnare()) + ", hihat: " + ofToString(beatDetector.isHat()) + ", sensitivity: " + ofToString(beatSensitivity/10.0f), 10, 58);
    ofEnableDepthTest();
}
