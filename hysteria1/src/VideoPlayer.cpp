#include "VideoPlayer.h"

//--------------------------------------------------------
void VideoPlayer::setup()
{
	ofSetVerticalSync(true);

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

    bikeVideo.load("videos/Victoria.mov");
    bikeVideo.setLoopState(OF_LOOP_NORMAL);
    bikeVideo.setSpeed(1.0);
    bikeVideo.play();

    papaVideo.load("videos/PapaSpeaksJapanese.MOV");
    papaVideo.setLoopState(OF_LOOP_NORMAL);
    papaVideo.setSpeed(1.0);
    papaVideo.play();
}

//--------------------------------------------------------
void VideoPlayer::update()
{
    video = (Video)vidChoice.get();

    switch (video)
    {
    case Video::BIKE:
        bikeVideo.update();
        if (bikeVideo.isFrameNew()) {
            ofPixels pixels = bikeVideo.getPixels();
            ofImage videoImg;
            videoImg.setFromPixels(pixels);
            ofxCv::convertColor(videoImg, gray, CV_RGB2GRAY);
            //ofxCv::Canny(gray, edge, 20, 20, 3);
            ofxCv::Sobel(gray, sobel, -1, 1, 1, 5, 2.0, 0.0, cv::BorderTypes::BORDER_DEFAULT);
            gray.update();
            //edge.update();
            sobel.update();

            /*edgePixels = edgeDetector.detectEdges(pixels);
            edgePlayer.update(edgePixels);*/
        }
        break;

    case Video::PAPA_SPEAKS_JAPANESE:
        papaVideo.update();
        break;
    }
}

//--------------------------------------------------------
void VideoPlayer::draw()
{
    float videoWidth, videoHeight;

    switch (video)
    {
    case Video::BIKE :
        videoWidth = bikeVideo.getWidth();
        videoHeight = bikeVideo.getHeight();
        break;
    
    case Video::PAPA_SPEAKS_JAPANESE :
        videoWidth = papaVideo.getWidth();
        videoHeight = papaVideo.getHeight();
        break;
    }

    float screenWidth = ofGetWidth();
    float screenHeight = ofGetHeight();

    float scale = min(screenWidth / videoHeight, screenHeight / videoWidth); // Swap width and height

    float drawWidth = videoHeight * scale; // Swap width and height
    float drawHeight = videoWidth * scale; // Swap width and height

    float x = (screenWidth - drawWidth) / 2.0f;
    float y = (screenHeight - drawHeight) / 2.0f;

    ofPushMatrix();
    ofTranslate(x + drawWidth / 2, y + drawHeight / 2); // Translate to center

    switch (video)
    {
    case Video::BIKE:
        ofRotate(90, 0, 0, 1); // Rotate 90 degrees clockwise
        sobel.draw(-drawHeight / 2, -drawWidth / 2, drawHeight, drawWidth); // Draw with swapped width and height
        break;

    case Video::PAPA_SPEAKS_JAPANESE:
        papaVideo.draw(-drawWidth / 2, -drawHeight / 2, drawWidth, drawHeight);
        break;
    }

    ofPopMatrix();


    //ofPixels& pixels = bikeVideo.getPixels();
    //int nChannels = pixels.getNumChannels();

    //// let's move through the "RGB(A)" char array
    //// using the red pixel to control the size of a circle.
    //for (int i = 4; i < vidWidth; i += 8) {
    //    for (int j = 4; j < vidHeight; j += 8) {
    //        unsigned char r = pixels[(j * 320 + i) * nChannels];
    //        float val = 1 - ((float)r / 255.0f);
    //        ofDrawCircle(400 + i, 20 + j, 10 * val);
    //    }
    //}

}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
