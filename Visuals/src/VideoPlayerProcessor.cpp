#include "VideoPlayerProcessor.h"
#include "AbstractProcessor.h"

//--------------------------------------------------------
void VideoPlayerProcessor::declareSettings()
{
    params.setName("Video Player");

    vidChoiceDD = std::make_unique<ofxIntDropdown>(vidChoice);
    for (int i = 0; i < numVideos; ++i)
    {
        vidChoiceDD->add(i, getVideoName(i));
    }
    vidChoiceDD->disableMultipleSelection();
    vidChoiceDD->enableCollapseOnSelection();
    vidChoiceDD->setName("Video Name");
    vidChoiceDD->setSelectedValueByIndex((int)currentVideo, false);

    drawStyleDD = std::make_unique<ofxIntDropdown>(drawStyle);
    drawStyleDD->add((int)Style::NORMAL, "Normal");
    drawStyleDD->add((int)Style::EDGE, "Edge");
    drawStyleDD->disableMultipleSelection();
    drawStyleDD->enableCollapseOnSelection();
    drawStyleDD->setName("Style");
    drawStyleDD->setSelectedValueByIndex((int)currentStyle, false);
}

//--------------------------------------------------------
void VideoPlayerProcessor::addDropdownsToPanel(std::shared_ptr<ofxPanel> panel)
{
    panel->add(vidChoiceDD.get());
    panel->add(drawStyleDD.get());
}

//--------------------------------------------------------
void VideoPlayerProcessor::setup()
{
    ofSetVerticalSync(true);

    for (int i = 0; i < numVideos - 1; ++i)
    {
        ofVideoPlayer player;
        player.load("videos/" + getVideoName(i));
        player.setLoopState(OF_LOOP_NORMAL);
        player.setVolume(0.0);
        player.setSpeed(1.0);
        player.play();
        videoPlayers.emplace_back(player);
    }

    vidChoice.addListener(this, &VideoPlayerProcessor::vidChoiceChanged);
    drawStyle.addListener(this, &VideoPlayerProcessor::drawStyleChanged);
}

//--------------------------------------------------------
void VideoPlayerProcessor::update()
{
    if (currentVideo == Video::NONE)
        return;

    videoPlayers[(int)currentVideo].update();

    if (currentStyle == Style::EDGE)
        edgeDetectVideo(videoPlayers[(int)currentVideo]);
}

//--------------------------------------------------------
void VideoPlayerProcessor::draw()
{
    if (currentVideo == Video::NONE)
        return;

    auto drawDims = getDrawWidthAndHeight(videoPlayers[(int)currentVideo]);

    float drawWidth = drawDims.first;
    float drawHeight = drawDims.second;
    float x = (ofGetWidth() - drawWidth) / 2.0f;
    float y = (ofGetHeight() - drawHeight) / 2.0f;

    ofPushMatrix();
    ofTranslate(x + drawWidth / 2, y + drawHeight / 2); // Translate to center

    switch (currentStyle)
    {
    case Style::NORMAL:
        videoPlayers[(int)currentVideo].draw(-drawWidth / 2, -drawHeight / 2, drawWidth, drawHeight);
        break;

    case Style::EDGE:
        sobelTexture.draw(-drawWidth / 2, -drawHeight / 2, drawWidth, drawHeight);
        break;
    }

    ofPopMatrix();
}

//-----------------------------------------------------
void VideoPlayerProcessor::edgeDetectVideo(ofVideoPlayer& videoPlayer)
{
    if (videoPlayer.isFrameNew()) 
    {
        ofPixels pixels = videoPlayer.getPixels();
        cv::Mat videoImg = ofxCv::toCv(pixels);
        cv::Mat gray;
        cv::cvtColor(videoImg, gray, cv::COLOR_RGB2GRAY);
        cv::Mat sobel;
        cv::Sobel(gray, sobel, -1, 1, 1, 5, 2.0, 0.0, cv::BorderTypes::BORDER_DEFAULT);

        ofPixels sobelPixels;
        ofxCv::toOf(sobel, sobelPixels);

        // Allocate texture only if not allocated or dimensions have changed
        if (!sobelTexture.isAllocated() || sobelTexture.getWidth() != sobelPixels.getWidth() || sobelTexture.getHeight() != sobelPixels.getHeight()) 
        {
            sobelTexture.allocate(sobelPixels.getWidth(), sobelPixels.getHeight(), GL_LUMINANCE);
        }

        sobelTexture.loadData(sobelPixels);
    }
}

//------------------------------------------------------
std::pair<const float, const float> VideoPlayerProcessor::getDrawWidthAndHeight(ofVideoPlayer& videoPlayer) const
{
    float videoWidth = videoPlayer.getWidth();
    float videoHeight = videoPlayer.getHeight();
    float screenWidth = ofGetWidth();
    float screenHeight = ofGetHeight();
    float screenAspectRatio = screenWidth / screenHeight;
    float videoAspectRatio = videoWidth / videoHeight;
    float drawWidth, drawHeight;

    if (screenAspectRatio > videoAspectRatio) {
        // The screen is wider than the video, so the video's height should fill the screen
        drawHeight = screenHeight;
        drawWidth = drawHeight * videoAspectRatio;
    }
    else {
        // The screen is narrower than the video, so the video's width should fill the screen
        drawWidth = screenWidth;
        drawHeight = drawWidth / videoAspectRatio;
    }

    return std::pair<const float, const float>(drawWidth, drawHeight);
}

//------------------------------------------------------
void VideoPlayerProcessor::vidChoiceChanged(int& vidChoice) 
{
    currentVideo = static_cast<Video>(vidChoice);
}

//------------------------------------------------------
void VideoPlayerProcessor::drawStyleChanged(int& drawStyle)
{
    currentStyle = static_cast<Style>(drawStyle);
}