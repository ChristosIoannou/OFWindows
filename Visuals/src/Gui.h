#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ProcessorFarm.h"
#include "ofJson.h"

class Gui
{
public:

	Gui() 
	{
		loadPositionsJson();
	};

	~Gui() 
	{
		savePositionsJson();
	};

	void generatePanels(ProcessorFarm& farm);
	void setup();
	void draw();

private:

	void loadPositionsJson();
	void savePositionsJson();
	std::pair<int, int> getPanelPositionForProcessor(const std::string& procName);
	std::vector<std::shared_ptr<ofxPanel>> panels;
	nlohmann::json positionsJson;
};

typedef std::unique_ptr<Gui> GuiPtr;