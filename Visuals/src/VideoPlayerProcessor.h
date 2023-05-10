#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "AbstractProcessor.h"
#include "ofxDropDown.h"

class VideoPlayerProcessor : public AbstractProcessor {

	enum class Video
	{
		BIKE,
		PAPA_SPEAKS_JAPANESE,
		FINGERS,
		NONE
	};

	enum class Style
	{
		NORMAL,
		EDGE,
		CIRCLES
	};

	constexpr static int numVideos = std::underlying_type_t<Video>(Video::NONE) + 1;
	constexpr static std::array<char*, numVideos> videoNames =
	{
		"Victoria.mov", "PapaSpeaksJapanese.MOV", "fingers.mov", "NONE"
	};

public:
	VideoPlayerProcessor() {};
	~VideoPlayerProcessor() {};

	virtual void setup() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void declareSettings() override;
	virtual void addDropdownsToPanel(std::shared_ptr<ofxPanel> panel) override;

	// Listeners
	void vidChoiceChanged(int& vidChoice);
	void drawStyleChanged(int& drawStyle);

	std::vector<ofVideoPlayer> videoPlayers;
	ofTexture sobelTexture;

	ofParameter<int> drawStyle;
	ofParameter<int> vidChoice;
	unique_ptr<ofxIntDropdown> vidChoiceDD;
	unique_ptr<ofxIntDropdown> drawStyleDD;

	Video currentVideo = Video::NONE;
	Style currentStyle = Style::NORMAL;

private:

	const std::string getVideoName(int video) const
	{
		return std::string{ videoNames[video] };
	}

	void edgeDetectVideo(ofVideoPlayer& videoPlayer);
	std::pair<const float, const float> getDrawWidthAndHeight(ofVideoPlayer& videoPlayer) const;

};