#include "ofApp.h"

int bark(float f) {
    float b = 13 * atan(0.00076 * f) + 3.5 * atan(pow(f / 7500.0f, 2));
    return ((int)floor(b));
}

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetFrameRate(-1);
    glPointSize(1.0);

    autoRotate = true;

    setupSoundStream();
    setupFFT();
    setupDancingMesh();

    setupAudioSphere();

}

//--------------------------------------------------------------
void ofApp::update() {

    updateFFTandAnalyse();

    // Time calculations
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time; //Store the current time

    if (b_dancingMesh)
        updateDancingMesh(dt);
    if (b_audioSphere)
        updateAudioSphere();

}

//--------------------------------------------------------------
void ofApp::draw() {

    ofBackground(ofColor::black);	//Set up the background
    ofEnableAlphaBlending();

    if (b_Info)
        drawInfo();

    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    if (b_dancingMesh)
        drawDancingMesh();
    if (b_audioSphere)
        drawAudioSphere();

    ofPopMatrix();
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
    case 'p':
        b_audioSphere = !b_audioSphere;
        break;
    //case '=':
    //    volumeMultiplier++;
    //    break;
    //case '-':
    //    volumeMultiplier--;
    //    break;
    case 'f':
        ofToggleFullscreen();
        break;
    case 'w':
        if (bandRad < SPECTRAL_BANDS)
            bandRad++;
        break;
    case 'q':
        if (bandRad > 0)
            bandRad--;
        break;
    case 's':
        if (bandVel < SPECTRAL_BANDS)
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

    // FFT/Spectrum
    paramsFFT.setName("FFT/Spectrum");
    paramsFFT.add(volumeMultiplier.set("Volume", 1, 0, 5));
    guiFFT.setup(paramsFFT);

    // DancingMesh
    paramsDancingMesh.setName("Dancing Mesh");
    paramsDancingMesh.add(b_dancingMesh.set("Draw", false));
    paramsDancingMesh.add(bandRad.set("Radius bin", 1, 0, SPECTRAL_BANDS));
    paramsDancingMesh.add(bandVel.set("Velocity bin", 10, 0, SPECTRAL_BANDS));
    guiDancingMesh.setup(paramsDancingMesh);

    // AudioSphere
    paramsAudioSphere.setName("Audio Sphere");
    paramsAudioSphere.add(b_audioSphere.set("Draw", true));
    paramsAudioSphere.add(autoRotate.set("Rotate", true));
    guiAudioSphere.setup(paramsAudioSphere);

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

    posShader.load("", "position.frag");       //load the position updating frag shader    
    fboResolution = sphereResolution;      //for the sphere we set this to the resolution which = #of verts along each axis

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


//==================== UPDATES =================================
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


//======================= DRAW =================================

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs& args) {
    guiFFT.draw();
    guiDancingMesh.draw();
    guiAudioSphere.draw();
}

//--------------------------------------------------------------
void ofApp::drawInfo() {
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
    }

    ofDrawBitmapString("screen      | fps: " + ofToString(ofGetFrameRate()), 10, 10);
    ofDrawBitmapString("soundStream | bufferSize: " + ofToString(soundStream.getBufferSize()) + ", sampleRate: " + ofToString(soundStream.getSampleRate()), 10, 22);
    ofDrawBitmapString("fft         | binSize: " + ofToString(fft->getBinSize()) + ", volume: " + ofToString(volumeMultiplier), 10, 34);
    ofDrawBitmapString("spectrum    | band[0] " + ofToString(spectrum[0]) + ", band[1]: " + ofToString(spectrum[1]), 10, 46);
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
        ofRotateX(startOffsetAngle += angleIncrement);
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
