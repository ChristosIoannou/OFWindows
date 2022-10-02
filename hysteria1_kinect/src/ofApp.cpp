#include "ofApp.h"
#include <cmath>
#include <random>

int bark(float f) {
    float b = 13 * atan(0.00076 * f) + 3.5 * atan(pow(f / 7500.0f, 2));
    return ((int)floor(b));
}

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetFrameRate(-1);
    glPointSize(1.0);

    setupSoundStream();
    setupFFT();
    setupKinect();
    setupFlashingText();
    setupPcExpand();
}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();

    // Time calculations
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time; //Store the current time

    if (b_kinect) // && !usePlayMesh)
        updateKinect();
    if (b_flashingText)
        updateFlashingText();
    if (b_pcExpand)
        updatePcExpand();

}

//--------------------------------------------------------------
void ofApp::draw() {

    ofBackground(ofColor::black);	//Set up the background

    if (b_kinect)
        drawKinect();

    ofEnableAlphaBlending();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    //cam.begin();

    drawFlashingText();

    //cam.end();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::exit() {
    soundStream.close();
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    switch (key) {
    case 'F':
    case 'f':
        ofToggleFullscreen();
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
    case ' ':
        usePlayMesh = !usePlayMesh;
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
        soundSpectrum[i] *= 0.95;	//Slow decreasing
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
    panelFFT.setup(paramsFFT, "settings.xml", 30, 265);

    // FlashingText
    paramsFlashingText.setName("Flashing Text");
    paramsFlashingText.add(b_flashingText.set("Draw FT", false));
    paramsFlashingText.add(inputMessage.set("Message", "THE QUEEN IS DEAD"));
    paramsFlashingText.add(markMax.set("Mark (s)", 0.25, 0, 1));
    paramsFlashingText.add(spaceMax.set("Space (s)", 1.2, 0, 1.5));
    paramsFlashingText.add(numFlashes.set("Number of flashes", 10, 0, 20));
    paramsFlashingText.add(framesLeft.set("Frames Left", 0, 0, 1200));
    panelFlashingText.setup(paramsFlashingText, "settings.xml", 490, 270);

    // Kinect
    paramsKinect.setName("Kinect");
    paramsKinect.add(b_kinect.set("Do", true));
    paramsKinect.add(bThreshWithOpenCV.set("Thresh with OpenCV", true));
    paramsKinect.add(bDrawPointCloud.set("PointCloud", true));
    paramsKinect.add(nearThreshold.set("Near thresh", 230, 0, 255));
    paramsKinect.add(farThreshold.set("Far thresh", 70, 0, 255));
    panelKinect.setup(paramsKinect, "settings.xml", 30, 380);

    // Expand point cloud
    paramsPcExpand.setName("PCL Expand");
    paramsPcExpand.add(b_pcExpand.set("Do", false));
    panelPcExpand.setup(paramsPcExpand, "settings.xml", 30, 620);

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
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    angle = 0;  // zero the tilt on startup
    kinect.setCameraTiltAngle(angle);
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    kinectPlayMesh.setMode(OF_PRIMITIVE_POINTS);
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
void ofApp::setupFlashingText() {
    eastBorderFont.load("East Border.ttf", 85);
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

//--------------------------------------------------------------
void ofApp::setupPcExpand() {
    b_pcExpand.addListener(this, &ofApp::transferKinectMeshToPlayMesh);
    b_pcExpand.addListener(this, &ofApp::remergePlayMeshToKinectMesh);
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
    //kinect.update();
    //// there is a new frame and we are connected
    //if (kinect.isFrameNew()) {

    //    // load grayscale depth image from the kinect source
    //    grayImage.setFromPixels(kinect.getDepthPixels());

    //    // we do two thresholds - one for the far plane and one for the near plane
    //    // we then do a cvAnd to get the pixels which are a union of the two thresholds
    //    if (bThreshWithOpenCV) {
    //        grayThreshNear = grayImage;
    //        grayThreshFar = grayImage;
    //        grayThreshNear.threshold(nearThreshold, true);
    //        grayThreshFar.threshold(farThreshold);
    //        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
    //    }
    //    else {

    //        // or we do it ourselves - show people how they can work with the pixels
    //        ofPixels& pix = grayImage.getPixels();
    //        int numPixels = pix.size();
    //        for (int i = 0; i < numPixels; i++) {
    //            if (pix[i] < nearThreshold && pix[i] > farThreshold) {
    //                pix[i] = 255;
    //            }
    //            else {
    //                pix[i] = 0;
    //            }
    //        }
    //    }

    //    // update the cv images
    //    grayImage.flagImageChanged();

    //    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    //    // also, find holes is set to true so we will get interior contours as well....
    //    contourFinder.findContours(grayImage, 10, (kinect.width * kinect.height) / 2, 20, false);
    //    getPointCloud();
    //}
    ofSpherePrimitive sphere;
    sphere.setRadius(200);
    sphere.setResolution(150);
    sphere.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    //kinectPC.setKinectMesh(sphere.getMesh());
    kinectMesh = sphere.getMesh();
    //std::cout << kinectMesh.getNumVertices() << std::endl;
}

//-------------------------------------------------------------
void ofApp::updateFlashingText() {

    b_flashingText = false;
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

//--------------------------------------------------------------
void ofApp::updatePcExpand() {

    for (int i = 0; i < kinectPlayMesh.getNumVertices(); ++i) {
        //kinectPlayMesh.getVertices()[i] = kinectPlayMesh.getVertices()[i] + kinectPlayMesh.getNormal(i) + pcDirections[i];
        kinectPlayMesh.getVertices()[i] = kinectPlayMesh.getVertices()[i] + pcDirections[i];
    }
}


//======================= DRAW =================================
//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs& args) {

    panelSpectra.draw();
    panelFFT.draw();
    panelFlashingText.draw();
    panelKinect.draw();
    panelPcExpand.draw();

    // draw fft spectrum
    if (drawSpectrum) {
        ofPushMatrix();
        ofTranslate(10, 20);
        ofSetLineWidth(1);
        ofNoFill();
        ofSetColor(ofColor::pink);
        ofDrawBitmapString("FFT Spectrum:", 0, 0);
        ofRect(0, 10, SPECTRAL_BANDS * 5, 100);
        //ofSetColor(ofColor::orange);
        ofSetColor(ofColor::lightGoldenRodYellow);
        for (int i = 0; i < SPECTRAL_BANDS; i++) {
            ofDrawRectangle(i * 5, 110, 3, -spectrum[i] * 50);
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

    ofSetColor(255, 255, 255);

    if (bDrawPointCloud) {
        cam.begin();
        drawPointCloud();
        cam.end();
    }
    else {
        // draw from the live kinect
        kinect.drawDepth(10, 10, 400, 300);
        kinect.draw(420, 10, 400, 300);

        grayImage.draw(10, 320, 400, 300);
        contourFinder.draw(10, 320, 400, 300);
    }
}

//--------------------------------------------------------------
void ofApp::drawFlashingText() {

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
            xshift = -(textWidth / 2) - (ofRandom(-1.0, 1.0) * (ofGetWidth() - textWidth)) / 10;
            yshift = -(ofRandom(-1.0, 1.0) * (ofGetHeight() - textHeight)) / 10;
        }

        if (flashIdx > flashFrames.size() - 1) {
            flashFrames.clear();
            flashIdx = 0;
        }

        flashIdx++;

    }
    framesLeft = flashFrames.size() - flashIdx;

}


//==================== HELPERS =================================
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
void ofApp::drawPointCloud() {

    ofSetColor(ofColor::white);
    glPointSize(1);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(1, -1, -1);
    //ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();
    //mesh.drawVertices();
    ofSetColor(ofColor::white);
    if (usePlayMesh)
        kinectPlayMesh.drawVertices();
    else
        kinectMesh.drawVertices();
    ofDisableDepthTest();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::getPointCloud() {

    kinectMesh.clear();
    int w = kinect.width;
    int h = kinect.height;
    int step = 1;
    for (int y = 0; y < h; y += step) {
        for (int x = 0; x < w; x += step) {
            if (kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1500) {
                //mesh.addColor(kinect.getColorAt(x, y));
                kinectMesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::transferKinectMeshToPlayMesh(bool &b_pcExpand) {
    usePlayMesh = b_pcExpand;
    if (usePlayMesh) {
        pcDirections.clear();
        kinectPlayMesh = kinectMesh;
        for (int i = 0; i < kinectPlayMesh.getNumVertices(); ++i) {
            ofVec3f direction(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
            pcDirections.push_back(direction);
        }
    }
}

//--------------------------------------------------------------
void ofApp::remergePlayMeshToKinectMesh(bool& b_pcExpand) {


}