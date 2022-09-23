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
    setupDancingMesh();
    setupAudioSphere();
    setupFlashingText();
}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();

    // Time calculations
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time; //Store the current time

    if (b_kinect)
        updateKinect();
    if (b_dancingMesh)
        updateDancingMesh(dt);
    if (b_flashingText)
        updateFlashingText();
    if (b_audioSphere)
        updateAudioSphere();

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

    if (b_dancingMesh)
        drawDancingMesh();

    drawFlashingText();

    if (b_audioSphere)
        drawAudioSphere();

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

    // DancingMesh
    paramsDancingMesh.setName("Dancing Mesh");
    paramsDancingMesh.add(b_dancingMesh.set("Draw DM", false));
    paramsDancingMesh.add(bandRad.set("Radius bin", 1, 0, SPECTRAL_BANDS));
    paramsDancingMesh.add(bandVel.set("Velocity bin", 10, 0, SPECTRAL_BANDS));
    panelDancingMesh.setup(paramsDancingMesh, "settings.xml", 260, 245);

    // AudioSphere
    paramsAudioSphere.setName("Audio Sphere");
    paramsAudioSphere.add(b_audioSphere.set("Draw AS", false));
    paramsAudioSphere.add(autoRotate.set("Rotate", true));
    paramsAudioSphere.add(rotationSpeed.set("Rotation Speed", 1, -2, 2));
    paramsAudioSphere.add(rotateSin.set("Sin Rotation", false));
    paramsAudioSphere.add(posDecayRate.set("Decay Rate", 0.995, 0.99, 0.9999));
    panelAudioSphere.setup(paramsAudioSphere, "settings.xml", 490, 160);

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
    paramsKinect.add(bDrawPointCloud.set("PointCloud", false));
    paramsKinect.add(nearThreshold.set("Near thresh", 230, 0, 255));
    paramsKinect.add(farThreshold.set("Far thresh", 70, 0, 255));
    panelKinect.setup(paramsKinect, "settings.xml", 30, 380);

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
void ofApp::setupDancingMesh() {
    p.resize(n);

    for (int i = 0; i < n; ++i) {
        tx.push_back(ofRandom(0, 1000));
        ty.push_back(ofRandom(0, 1000));
    }
}

//--------------------------------------------------------------
void ofApp::setupAudioSphere() {
    //generate the mesh points
    buildSphereMesh(sphereRadius, sphereResolution, vm);
    cout << "nverts: " << vm.getNumVertices() << endl;
    cout << "arb: " << ofGetUsingArbTex() << ", norm: " << ofGetUsingNormalizedTexCoords() << endl;

    shader.load("tex.vert", "tex.frag");        //load the texture shader    
    float freq_spc = FREQ_MAX / (float)SPECTRAL_BANDS;  //map the frequencies to bark bands

    for (int i = 0; i < SPECTRAL_BANDS; i++) {
        int bidx = bark(i * freq_spc);
        barkmap[i] = bidx;
    }

    fboResolution = sphereResolution;      //for the sphere we set this to the resolution which = #of verts along each axis
    posShader.load("", "position.frag");       //load the position updating frag shader    
   
    //init the fbo's with blank data
    vector<ofVec3f> fbo_init_data;
    fbo_init_data.assign(fboResolution * fboResolution, ofVec3f(0.0, 0.0, 0.0));

    posBuffer.allocate(fboResolution, fboResolution, GL_RGB32F);
    posBuffer.src->getTextureReference().loadData((float*)&fbo_init_data[0], fboResolution, fboResolution, GL_RGB);
    posBuffer.dst->getTextureReference().loadData((float*)&fbo_init_data[0], fboResolution, fboResolution, GL_RGB);

    //reuse fbo_init_data for no real reason, it just needs to be blank
    eqTexture.allocate(fboResolution, 1, GL_RGB32F_ARB);
    eqTexture.loadData((float*)&fbo_init_data[0], fboResolution, 1, GL_RGB);

    axisLocation = fboResolution;
    angleIncrement = 180.0 / (float)fboResolution;
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
    if (kinect.isFrameNew()) {

        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if (bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        }
        else {

            // or we do it ourselves - show people how they can work with the pixels
            ofPixels& pix = grayImage.getPixels();
            int numPixels = pix.size();
            for (int i = 0; i < numPixels; i++) {
                if (pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                }
                else {
                    pix[i] = 0;
                }
            }
        }

        // update the cv images
        grayImage.flagImageChanged();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 10, (kinect.width * kinect.height) / 2, 20, false);
    }
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
void ofApp::updateAudioSphere() {

    /* update our bark map'd frequency bins */
    memset(bins, 0x00, sizeof(bins));
    for (int i = 0; i < SPECTRAL_BANDS; i++) {
        int idx = barkmap[i];
        bins[idx] += spectrum[i] * 70;
    }

    /* put the eq vals into a path to turn them into a curve */
    int line_len = fboResolution;
    float ppseg = line_len / (float)(BARK_MAX + 1);
    eqPath.clear();
    eqPath.curveTo(0, -bins[0]);
    for (int i = 0; i < BARK_MAX; i++) {
        eqPath.curveTo(i * ppseg, -bins[i]);
    }

    //smooth this out a little at the end so the eq texture wraps, 25 = BARK_MAX
    //eqPath.curveTo(25 * ppseg, -(bins[0] + bins[BARK_MAX] + bins[BARK_MAX - 1] + bins[BARK_MAX - 2]) / 4.0f);
    //eqPath.curveTo(26 * ppseg, -bins[0]);
    //eqPath.curveTo(26 * ppseg, -bins[0]);

    ofMesh eq_m = eqPath.getTessellation();

    eqTexture.loadData((float*)eq_m.getVerticesPointer(), fboResolution, 1, GL_RGB); //load up the eq curve into a texture
    axisLocation--;     //update where on the axis we will apply the latest eq data

    if (axisLocation < 0)
        axisLocation = fboResolution;

    //use fbo to work out displacement coeffcients
    posBuffer.dst->begin();
    ofClear(0);
    posShader.begin();
    posShader.setUniformTexture("u_prevDisp", posBuffer.src->getTextureReference(), 0);
    posShader.setUniformTexture("u_newDisp", eqTexture, 1); //pass the new displacement data
    posShader.setUniform1f("u_axisloc", axisLocation);
    posShader.setUniform1f("u_decayRate", posDecayRate);

    ofSetColor(255, 255, 255, 255);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(fboResolution, 0); glVertex3f(fboResolution, 0, 0);
    glTexCoord2f(fboResolution, fboResolution); glVertex3f(fboResolution, fboResolution, 0);
    glTexCoord2f(0, fboResolution); glVertex3f(0, fboResolution, 0);
    glEnd();

    posShader.end();
    posBuffer.dst->end();
    posBuffer.swap();
}

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

//======================= DRAW =================================
//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs& args) {
    
    panelSpectra.draw();
    panelFFT.draw();
    panelDancingMesh.draw();
    panelAudioSphere.draw();
    panelFlashingText.draw();
    panelKinect.draw();

    // draw fft spectrum
    if (drawSpectrum) {
        ofPushMatrix();
        ofTranslate(10, 20);
        ofSetLineWidth(1);
        ofNoFill();
        ofSetColor(ofColor::pink);
        ofDrawBitmapString("FFT Spectrum:", 0, 0);
        ofRect(0, 10, SPECTRAL_BANDS * 5, 100);
        ofSetColor(ofColor::orange);
        for (int i = 0; i < SPECTRAL_BANDS; i++) {
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
            ofDrawRectangle(i * 5, 110, 3, -spectrum[i] * 50);
        }
        ofPopMatrix();
    }

    // draw bark spectrum
    if (drawBark) {
        ofPushMatrix();
        ofTranslate(280, 20);
        ofDrawBitmapString("Bark Spectrum:", 0, 0);
        ofRect(0, 10, BARK_MAX * 10, 100);
        ofSetColor(ofColor::orange);
        for (int i = 0; i < BARK_MAX; i++) {
            //Draw bandRad and bandVel by black color,
            //and other by gray color
            ofSetColor(ofColor::pink); //Gray color
            ofDrawRectangle(i * 10, 110, 3, -bins[i] * 0.6);
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
void ofApp::drawAudioSphere() {
    //cam.begin();

    if (autoRotate) {
        ofRotateY(-30);
        if (rotateSin)
            rotationSpeed = 2 * sin(ofGetFrameNum() * 0.005);
        ofRotateX(startOffsetAngle += angleIncrement * rotationSpeed);
        ofRotateZ(ofGetFrameNum() * 0.005);
    }
    else {
        ofRotateX(0.0);
        ofRotateY(0.0);
        ofRotateZ(0.0);
    }

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    shader.begin();
    shader.setUniformTexture("u_OffMap", posBuffer.src->getTextureReference(), 0);
    shader.setUniform1f("u_fboRes", (float)fboResolution);

    vm.drawVertices();
    shader.end();
    glDisable(GL_POINT_SMOOTH);

    //cam.end();
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
            xshift = - (textWidth / 2) - (ofRandom(-1.0, 1.0) * (ofGetWidth() - textWidth)) / 10;
            yshift =  - (ofRandom(-1.0, 1.0) * (ofGetHeight() - textHeight)) / 10;
        }

        if (flashIdx > flashFrames.size() - 1) {
            flashFrames.clear();
            flashIdx = 0;
            b_audioSphere = true;
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
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableIndices();
    int step = 1;
    for (int y = 0; y < h; y += step) {
        for (int x = 0; x < w; x += step) {
            if (kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1500) {
                //mesh.addColor(kinect.getColorAt(x, y));
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }

    glPointSize(1);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();
    //mesh.drawVertices();
    mesh.draw();
    ofDisableDepthTest();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::buildSphereMesh(int radius, int sphereResolution, ofMesh& sphereMesh) {

    int n = sphereResolution * 2;
    float ndiv2 = (float)n / 2;

    float theta2 = TWO_PI;
    float phi1 = -HALF_PI;
    float phi2 = HALF_PI;
    float r = radius;
    sphereMesh.clear();
    int i, j;
    float theta1 = 0.f;
    float jdivn, j1divn, idivn, dosdivn, unodivn = 1 / (float)n, t1, t2, t3, cost1, cost2, cte1, cte3;
    cte3 = (theta2 - theta1) / n;
    cte1 = (phi2 - phi1) / ndiv2;
    dosdivn = 2 * unodivn;
    ofVec3f e, p, e2, p2;

    if (n < 0) {
        n = -n;
        ndiv2 = -ndiv2;
    }
    if (n < 4) { n = 4; ndiv2 = (float)n / 2; }
    if (r <= 0) r = -r;

    t2 = phi1;
    cost2 = cos(phi1);
    j1divn = 0;

    ofVec3f vert, normal;
    ofVec2f tcoord;

    for (j = 0; j < ndiv2; j++) {
        t1 = t2;
        t2 += cte1;
        t3 = theta1 - cte3;
        cost1 = cost2;
        cost2 = cos(t2);
        e.y = sin(t1);
        e2.y = sin(t2);
        p.y = r * e.y;
        p2.y = r * e2.y;

        idivn = 0;
        jdivn = j1divn;
        j1divn += dosdivn;
        for (i = 0; i <= n; i++) {
            t3 += cte3;
            e.x = cost1 * cos(t3);
            e.z = cost1 * sin(t3);
            p.x = r * e.x;
            p.z = r * e.z;

            normal.set(e.x, e.y, e.z);
            tcoord.set(idivn, jdivn);
            vert.set(p.x, p.y, p.z);

            sphereMesh.addNormal(normal);
            sphereMesh.addTexCoord(tcoord);
            sphereMesh.addVertex(vert);

            e2.x = cost2 * cos(t3);
            e2.z = cost2 * sin(t3);
            p2.x = r * e2.x;
            p2.z = r * e2.z;

            normal.set(e2.x, e2.y, e2.z);
            tcoord.set(idivn, j1divn);
            vert.set(p2.x, p2.y, p2.z);

            sphereMesh.addNormal(normal);
            sphereMesh.addTexCoord(tcoord);
            sphereMesh.addVertex(vert);

            idivn += unodivn;

        }
    }
}
