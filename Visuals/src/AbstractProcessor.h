#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

class AbstractProcessor {

public:

	AbstractProcessor() {}
	~AbstractProcessor() {};

	virtual void setupProcessor()
	{
		declareSettings();
		setup();
	};

	virtual void addDropdownsToPanel(std::shared_ptr<ofxPanel> panel)
	{};

	virtual void declareSettings() = 0;
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	ofParameterGroup params;

};

typedef std::shared_ptr<AbstractProcessor> AbstractProcessorPtr;