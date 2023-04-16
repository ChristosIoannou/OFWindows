#include "ofApp.h"
#include <cmath>
#include <random>

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetFrameRate(-1);
    glPointSize(1.0);
    ofBackground(ofColor::black);	//Set up the background

    doController.setup();
    setupSoundStream();
    setupFFT();
    setupKinect();
    setupAudioSphere();
    setupFlashingText();
    setupKinectPointCloud();
    setupParticleRiver();
    setupKinectContour();
    setupTunnel();
    setupSurfaceMesh();
    setupVideoPlayer();
    setupParabolicCurve();
}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();

    if (doController.b_kinect)
        updateKinect();
    if (doController.b_flashingText)
        updateFlashingText();
    if (doController.b_kinectPointCloud)
        updateKinectPointCloud();
    if (doController.b_audioSphere)
        updateAudioSphere();
    if (doController.b_particleRiver)
        updateParticleRiver();
    if (doController.b_kinectContour)
        updateKinectContour();
    if (doController.b_tunnel)
        updateTunnel();
    if (doController.b_surfaceMesh)
        updateSurfaceMesh();
    if (doController.b_videoPlayer)
        updateVideoPlayer();
    if (doController.b_parabolicCurve)
        updateParabolicCurve();
}

//--------------------------------------------------------------
void ofApp::draw() {

    ofEnableAlphaBlending();
    drawFlashingText();
    if (doController.b_kinectPointCloud)
        drawKinectPointCloud();
    if (doController.b_audioSphere)
        drawAudioSphere();
    if (doController.b_kinectContour)
        drawKinectContour();
    if (doController.b_particleRiver)
        drawParticleRiver();
    if (doController.b_tunnel)
        drawTunnel();
    if (doController.b_surfaceMesh)
        drawSurfaceMesh();
    if (doController.b_videoPlayer)
        drawVideoPlayer();
    if (doController.b_parabolicCurve)
        drawParabolicCurve();
}

//--------------------------------------------------------------
void ofApp::exit() {
    soundStream.close();
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
    particleRiver.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    switch (key) {
    case 'F':
    case 'f':
        ofToggleFullscreen();
        if (doController.b_particleRiver)
            particleRiver.resize();
        if (doController.b_tunnel)
            tunnel.resize();
        if (doController.b_parabolicCurve)
            parabolicCurve.resize();
        break;
    case OF_KEY_UP:
        angle++;
        if (angle > 30) angle = 30;
        kinect.setCameraTiltAngle(angle);
        break;
    case OF_KEY_DOWN:
        angle--;
        if (angle < -30) angle = -30;
        kinect.setCameraTiltAngle(angle);
        break;
    case 'm':
        particleRiver.drawMap = !particleRiver.drawMap;
        break;
    case 'p':
        particleRiver.drawParts = !particleRiver.drawParts;
        break;
    case '1':
        particleRiver.spread = 1.0;
        break;
    case '2':
        particleRiver.spread = 2.0;
        break;
    case '3':
        particleRiver.spread = 3.0;
        break;
    case '8':
        particleRiver.speed = 1.5;
        particleRiver.spread = 8.0;
        break;
    case '0':
        particleRiver.spread = 10.0;
        break;
    }
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
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {

    soundMutex.lock();
    audioInput = input.getBuffer().data();
    fft->setSignal(audioInput);
    memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * SPECTRAL_BANDS);

    for (int i = 0; i < SPECTRAL_BANDS; i++) {
        fftOutput[i] = fftOutput[i] * volumeMultiplier;
    }

    ////Update our smoothed spectrum
    for (int i = 0; i < SPECTRAL_BANDS; i++) {
        soundSpectrum[i] *= fftDecay;	//Slow decreasing
        soundSpectrum[i] = max(soundSpectrum[i], fftOutput[i]);
    }
    soundMutex.unlock();
}

//==================== SETUPS ==================================
//--------------------------------------------------------------
void ofApp::setupGui() {

    // GUI
    paramsSpectra.setName("Draw on GUI");
    paramsSpectra.add(drawSpectrum.set("Spectrum", true));
    paramsSpectra.add(drawBark.set("Bark", true));
    paramsSpectra.add(drawSignal.set("Signal", true));
    panelSpectra.setup(paramsSpectra, "settings.xml", 30.0, 160.0);

    // FFT/Spectrum
    paramsFFT.setName("FFT/Spectrum");
    paramsFFT.add(volumeMultiplier.set("Volume", 2, 0, 8));
    paramsFFT.add(fftDecay.set("Decay", 0.95, 0.8, 1.0));
    paramsFFT.add(bassMid.set("BassMid", 3, 0, 48));
    paramsFFT.add(midHigh.set("MidHigh", 20, 0, 48));
    panelFFT.setup(paramsFFT, "settings.xml", 30, 265);

    // AudioSphere
    paramsAudioSphere.setName("Audio Sphere");
    paramsAudioSphere.add(doController.b_audioSphere.set("Draw AS", false));
    paramsAudioSphere.add(autoRotate.set("Rotate", true));
    paramsAudioSphere.add(rotationSpeed.set("Rotation Speed", 1, -2, 2));
    paramsAudioSphere.add(rotateSin.set("Sin Rotation", false));
    paramsAudioSphere.add(posDecayRate.set("Decay Rate", 0.995, 0.99, 0.9999));
    panelAudioSphere.setup(paramsAudioSphere, "settings.xml", 490, 160);

    // FlashingText
    paramsFlashingText.setName("Flashing Text");
    paramsFlashingText.add(doController.b_flashingText.set("Draw FT", false));
    paramsFlashingText.add(inputMessage.set("Message", "THE QUEEN IS DEAD"));
    paramsFlashingText.add(markMax.set("Mark (s)", 0.25, 0, 1));
    paramsFlashingText.add(spaceMax.set("Space (s)", 1.2, 0, 1.5));
    paramsFlashingText.add(numFlashes.set("Number of flashes", 10, 0, 20));
    paramsFlashingText.add(framesLeft.set("Frames Left", 0, 0, 1200));
    panelFlashingText.setup(paramsFlashingText, "settings.xml", 490, 300);

    // Kinect
    paramsKinect.setName("Kinect");
    paramsKinect.add(doController.b_kinect.set("Do", false));
    paramsKinect.add(kinectContour.nearThreshold.set("Near thresh", 230, 0, 255));
    paramsKinect.add(kinectContour.farThreshold.set("Far thresh", 210, 0, 255));
    panelKinect.setup(paramsKinect, "settings.xml", 30, 390);

    // Expand point cloud
    paramsKinectPointCloud.setName("Kinect Point Cloud");
    paramsKinectPointCloud.add(doController.b_kinectPointCloud.set("Do", false));
    paramsKinectPointCloud.add(kinectPointCloud.b_explode.set("Explode", false));
    paramsKinectPointCloud.add(kinectPointCloud.b_remerge.set("Remerge", false));
    paramsKinectPointCloud.add(kinectPointCloud.lowerThresh.set("LowerThresh", 0.0f, 0.0f, 10000.0f));
    paramsKinectPointCloud.add(kinectPointCloud.upperThresh.set("UpperThresh", 3000.0f, 0.0f, 10000.0f));
    paramsKinectPointCloud.add(kinectPointCloud.colorScheme.set("ColorScheme", 0, 0, 2));
    paramsKinectPointCloud.add(kinectPointCloud.b_flash.set("Flash", false));
    paramsKinectPointCloud.add(kinectPointCloud.flashPeriod.set("FlashPeriod", 10, 0, 20));
    paramsKinectPointCloud.add(kinectPointCloud.b_wave.set("Wave", false));
    paramsKinectPointCloud.add(kinectPointCloud.wave_amplitude.set("Wave Amplitude", 50, 0, 100));
    paramsKinectPointCloud.add(kinectPointCloud.wave_freq.set("Wave Frequency", 2, 0.0, 5.5));
    panelKinectPointCloud.setup(paramsKinectPointCloud, "settings.xml", 30, 490);

    // ParticleRiver
    paramsParticleRiver.setName("Particle River");
    paramsParticleRiver.add(doController.b_particleRiver.set("Do", false));
    paramsParticleRiver.add(particleRiver.drawMap.set("Draw Map", false));
    paramsParticleRiver.add(particleRiver.spread.set("Spread", 1.0f, 0.0f, 20.0f));
    paramsParticleRiver.add(particleRiver.speed.set("Speed", 2.5f, 0.0f, 20.0f));
    paramsParticleRiver.add(particleRiver.circ_coeff.set("Circle Coefficient", 0.01f, 0.0f, 0.5f));    
    paramsParticleRiver.add(particleRiver.spreadSin.set("Spread Sin", false));
    paramsParticleRiver.add(particleRiver.speedSin.set("Speed Sin", false));
    paramsParticleRiver.add(particleRiver.circ_coeffSin.set("Circ Coeff Sin", false));
    panelParticleRiver.setup(paramsParticleRiver, "settings.xml", 260, 245);

    // KinectContour
    paramsKinectContour.setName("Kinect Contour");
    paramsKinectContour.add(doController.b_kinectContour.set("Do", false));
    paramsKinectContour.add(kinectContour.continuousConcentric.set("Continuous Concentric", true));
    paramsKinectContour.add(kinectContour.nContours.set("Num Contours", 5, 0, 10));
    paramsKinectContour.add(kinectContour.sizeRatio.set("Size Ratio", 0.7, 0, 2));
    panelKinectContour.setup(paramsKinectContour, "settings.xml", 30, 580);

    // Tunnel
    paramsTunnel.setName("Tunnel");
    paramsTunnel.add(doController.b_tunnel.set("Do", false));
    paramsTunnel.add(tunnel.spacing.set("Spacing", 15, 0, 20));
    panelTunnel.setup(paramsTunnel, "settings.xml", 260, 445);

    // SurfaceMesh
    paramsSurfaceMesh.setName("SurfaceMesh");
    paramsSurfaceMesh.add(doController.b_surfaceMesh.set("Do", false));
    paramsSurfaceMesh.add(surfaceMesh.usePerlin.set("Perlin", true));
    paramsSurfaceMesh.add(surfaceMesh.alpha.set("Alpha", 0.3, 0.0, 1.0));
    paramsSurfaceMesh.add(surfaceMesh.rangeDependent.set("Range Dependent", false));
    paramsSurfaceMesh.add(surfaceMesh.colorScheme.set("Color Scheme", 2, 0, 2));
    paramsSurfaceMesh.add(surfaceMesh.sinCircle.set("Sin Circle", false));
    paramsSurfaceMesh.add(surfaceMesh.rotateZ.set("RotateZ", false));
    paramsSurfaceMesh.add(surfaceMesh.rotateX.set("RotateX", false));
    paramsSurfaceMesh.add(surfaceMesh.rotateY.set("RotateY", false));
    paramsSurfaceMesh.add(surfaceMesh.zoom.set("Zoom", false));
    paramsSurfaceMesh.add(surfaceMesh.wireframe.set("Wireframe", true));
    panelSurfaceMesh.setup(paramsSurfaceMesh, "settings.xml", 490, 465);

    // VideoPlayer
    paramsVideoPlayer.setName("VideoPlayer");
    paramsVideoPlayer.add(doController.b_videoPlayer.set("Do", false));
    paramsVideoPlayer.add(videoPlayer.vidChoice.set("Video", 0, 0, 2));
    panelVideoPlayer.setup(paramsVideoPlayer, "settings.xml", 260, 500);

    // ParabolicCurve
    paramsParabolicCurve.setName("ParabolicCurve");
    paramsParabolicCurve.add(doController.b_parabolicCurve.set("Do", false));
    paramsParabolicCurve.add(parabolicCurve.regenerate.set("Regenerate", false));
    paramsParabolicCurve.add(parabolicCurve.flash.set("Flash", false));
    paramsParabolicCurve.add(parabolicCurve.spacing.set("Spacing", 15, 0, 20));
    paramsParabolicCurve.add(parabolicCurve.linesMode.set("Lines Mode", 0, 0, 1));
    panelParabolicCurve.setup(paramsParabolicCurve, "settings.xml", 260, 600);

    ofSetBackgroundColor(0);
}

//--------------------------------------------------------------
void ofApp::setupSoundStream() {
    ofSoundStreamSettings settings;
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
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::setupKinect() {

    kinect.setRegistration(true);   // enable depth->video image calibration
    kinect.init();  // shows normal RGB video image
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    kinect.open();

    angle = 0;  // zero the tilt on startup
    kinect.setCameraTiltAngle(angle);
}

//--------------------------------------------------------------
void ofApp::setupFFT() {
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
    audioInput = new float[bufferSize];
    fftOutput = new float[SPECTRAL_BANDS];
    spectrum.resize(SPECTRAL_BANDS, 0.0f);
    soundSpectrum.resize(SPECTRAL_BANDS, 0.0f);
}

//--------------------------------------------------------------
void ofApp::setupAudioSphere() {
    audioSphere.setup();
}

//--------------------------------------------------------------
void ofApp::setupFlashingText() {
    eastBorderFont.load("East Border.ttf", 85);
}

//--------------------------------------------------------------
void ofApp::setupKinectPointCloud() {
    kinectPointCloud.setup();
}

//--------------------------------------------------------------
void ofApp::setupBeatDetector() {
    FFTHistoryMaxSize = FREQ_MAX / bufferSize;
    int bandSize = FFTHistoryMaxSize;
    beatDetectorBandLimits.clear();
    beatDetectorBandLimits.reserve(4);

    // BASS 60hz - 130 Hz (Kick)
    beatDetectorBandLimits.push_back(60 / bandSize);
    beatDetectorBandLimits.push_back(130 / bandSize);

    // LOW-MIDRange 301 - 750Hz (Snare)
    beatDetectorBandLimits.push_back(301 / bandSize);
    beatDetectorBandLimits.push_back(750 / bandSize);

    beatDetector.clear();
}

void ofApp::setupParticleRiver() {
    particleRiver.setup();
}

void ofApp::setupKinectContour() {
    kinectContour.setup();
}

void ofApp::setupTunnel() {
    tunnel.setup();
}

void ofApp::setupSurfaceMesh() {
    surfaceMesh.setup();
}

void ofApp::setupVideoPlayer() {
    videoPlayer.setup();
}

void ofApp::setupParabolicCurve() {
    parabolicCurve.setup();
}

//==================== UPDATES =================================
//--------------------------------------------------------------
void ofApp::updateFFTandAnalyse() {
    soundMutex.lock();
    spectrum = soundSpectrum;
    soundMutex.unlock();
    analyseFFT();
}

//--------------------------------------------------------------
void ofApp::updateBeatDetector() {


}

//--------------------------------------------------------------
void ofApp::updateKinect() {
    kinect.update();
    // there is a new frame and we are connected

    //ofSpherePrimitive sphere;
    //sphere.setRadius(200);
    //sphere.setResolution(150);
    //sphere.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    //kinectPointCloud.setKinectMesh(sphere.getMesh());
    //kinectPointCloud.kinectMesh = sphere.getMesh();
}

//--------------------------------------------------------------
void ofApp::updateKinectPointCloud() {
    kinectPointCloud.update();
}

//--------------------------------------------------------------
void ofApp::updateAudioSphere() {
    audioSphere.update(spectrum, posDecayRate);
}

void ofApp::updateFlashingText() {

    doController.b_flashingText = false;
    startingFrameNumber = ofGetFrameNum();
    flashFrames.clear();
    for (int i = 0; i < numFlashes; ++i) {
        int mark = ofRandom(markMax * 60);      // maybe try to use some kind of Poisson distribution here ?
        int space = ofRandom(spaceMax * 60);    // maybe try to use some kind of Poisson distribution here ?
        
        while (mark != 0) {
            flashFrames.push_back(1);
            mark--;
        }
        while (space != 0) {
            flashFrames.push_back(0);
            space--;
        }
    }
}

void ofApp::updateParticleRiver() {
    particleRiver.update();
}

void ofApp::updateKinectContour() {
    kinectContour.update();
}

void ofApp::updateTunnel() {
    tunnel.update();
}

void ofApp::updateSurfaceMesh() {
    surfaceMesh.update(bass, mids, highs);
}

void ofApp::updateVideoPlayer() {
    videoPlayer.update();
}

void ofApp::updateParabolicCurve() {
    parabolicCurve.update();
}

//======================= DRAW =================================
//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs& args) {
    
    panelSpectra.draw();
    panelFFT.draw();
    panelAudioSphere.draw();
    panelFlashingText.draw();
    panelKinect.draw();
    panelKinectPointCloud.draw();
    panelParticleRiver.draw();
    panelKinectContour.draw();
    panelTunnel.draw();
    panelSurfaceMesh.draw();
    panelVideoPlayer.draw();
    panelParabolicCurve.draw();

    // draw fft spectrum
    if (drawSpectrum) {
        ofPushMatrix();
        ofTranslate(10, 20);
        ofSetLineWidth(1);
        ofNoFill();
        ofSetColor(ofColor::pink);
        ofDrawBitmapString("FFT Spectrum:", 0, 0);
        ofRect(0, 10, SPECTRAL_BANDS * 5, 100);
        //ofSetColor(ofColor::lightGoldenRodYellow); //Gray color
        for (int i = 0; i < SPECTRAL_BANDS; i++) {
            if (i < bassMid)
                ofSetColor(ofColor::red);
            else if (i < midHigh)
                ofSetColor(ofColor::green);
            else
                ofSetColor(ofColor::blue);

            ofDrawRectangle(i * 5, 110, 3, -spectrum[i] * 50);
        }
        ofPopMatrix();
    }

    // draw bark spectrum
    if (drawBark) {
        ofPushMatrix();
        ofTranslate(280, 20);
        ofSetColor(ofColor::orange);
        ofDrawBitmapString("Bark Spectrum:", 0, 0);
        ofRect(0, 10, BARK_MAX * 10, 100);
        for (int i = 0; i < BARK_MAX; i++) {
            //Draw bandRad and bandVel by black color,
            //and other by gray color
            ofSetColor(ofColor::pink); //Gray color
            ofDrawRectangle(i * 10, 110, 3, -audioSphere.bins[i] * 0.6);
        }
        ofPopMatrix();
    }

    // draw buffer
    if (drawSignal) {
        ofPushMatrix();
        ofTranslate(500, 20);
        ofDrawBitmapString("Signal:", 0, 0);
        ofRect(0, 10, bufferSize, 100);
        ofSetColor(ofColor::orange);
        ofBeginShape();
        for (int i = 0; i < bufferSize; i++) {
            float signalValue = abs(fft->getSignal()[i] * 60) < 42 ? fft->getSignal()[i] * 60 : 42;
            ofVertex(i, 60 - signalValue);
            //std::cout << fft->getSignal()[i] * 60 << std::endl;
        }
        ofEndShape(false);
        ofPopMatrix();
    }

    ofPushMatrix();
    ofTranslate(270, 170);
    float fps = ofGetFrameRate();
    float fpsHue = ofMap(fps, 20, 60, 0, 85);
    ofColor fpsColor;
    fpsColor.setHsb(fpsHue, 255, 255);
    ofSetColor(fpsColor);
    ofRect(0, 0, 180, 40);
    ofDrawBitmapString("FPS: " + ofToString(fps), 45, 25);
    ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::drawKinect() {

    // draw from the live kinect
    //kinect.drawDepth(10, 10, 400, 300);
    //kinect.draw(420, 10, 400, 300);

}

//--------------------------------------------------------------
void ofApp::drawKinectPointCloud() {

    kinectPointCloud.draw(cam);
}

//--------------------------------------------------------------
void ofApp::drawAudioSphere() {
    audioSphere.draw(autoRotate, rotationSpeed, rotateSin);
}

//--------------------------------------------------------------
void ofApp::drawFlashingText() {
    ofDisableDepthTest();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    if (!flashFrames.empty()) {

        std::string text = inputMessage.get();

        if (flashIdx < flashFrames.size() && flashFrames[flashIdx] == 1) {

            ofColor textColor;
            if (colorRand < 1.0) 
                textColor.set(ofColor::mediumSeaGreen);     //textColor.setHex(0x51DCAA, 1.0f);
            else 
                textColor.set(ofColor::mediumPurple);

            ofSetColor(textColor);
            eastBorderFont.drawString(text, xshift, yshift);
        }
        else {
            colorRand = ofRandom(2.0);
            float textWidth = eastBorderFont.stringWidth(text);
            float textHeight = eastBorderFont.stringHeight(text);
            xshift = - (textWidth / 2) - (ofRandom(-1.0, 1.0) * (ofGetWidth() - textWidth)) / 10;
            yshift =  - (ofRandom(-1.0, 1.0) * (ofGetHeight() - textHeight)) / 10;
        }

        if (flashIdx > flashFrames.size() - 1) {
            flashFrames.clear();
            flashIdx = 0;
        }

        flashIdx++;
     
    }
    framesLeft = flashFrames.size() - flashIdx;
    ofPopMatrix();
}

void ofApp::drawParticleRiver() {
    particleRiver.draw();
}

void ofApp::drawKinectContour() {
    kinectContour.draw(spectrum[1]);
}

void ofApp::drawTunnel() {
    tunnel.draw();
}

void ofApp::drawSurfaceMesh() {
    surfaceMesh.draw();
}

void ofApp::drawVideoPlayer() {
    videoPlayer.draw();
}

void ofApp::drawParabolicCurve() {
    parabolicCurve.draw();
}

//==================== HELPERS =================================
//--------------------------------------------------------------
void ofApp::analyseFFT() {

    //Calculate color based on range
    midHigh = midHigh <= bassMid ? bassMid : midHigh;
    std::vector<float>::iterator rg_it = std::next(spectrum.begin(), bassMid);
    std::vector<float>::iterator gb_it = std::next(spectrum.begin(), midHigh);

    bass = std::accumulate(spectrum.begin(), rg_it, 0.0f);
    mids = std::accumulate(rg_it, gb_it, 0.0f);
    highs = std::accumulate(gb_it, spectrum.end(), 0.0f);
    totals = std::accumulate(spectrum.begin(), spectrum.end(), 0.0f);

}
