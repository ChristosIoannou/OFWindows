#pragma once

#include "ofMain.h"
#include "Gui.h"

void Gui::generatePanels(ProcessorFarm& farm)
{
	for (const auto& processor : farm.getProcessors())
	{
		auto panel = std::make_shared<ofxPanel>();

		std::string procName = processor->params.getName();
		auto panelPos = getPanelPositionForProcessor(procName);

		panel->setup(processor->params, "settings.xml", panelPos.first, panelPos.second);
		panel->setName(procName);
		processor->addDropdownsToPanel(panel);
		panels.emplace_back(panel);
	}
}

void Gui::setup()
{
	ofSetBackgroundColor(0);
}

void Gui::draw()
{
	for (auto& panel : panels)
	{
		panel->draw();
	}
}

void Gui::loadPositionsJson()
{
	positionsJson = ofLoadJson("positions.json");
}

void Gui::savePositionsJson()
{
	for (auto& panel : panels)
	{
		positionsJson[panel->getName()] = { panel->getPosition().x, panel->getPosition().y };
	}

	ofSaveJson("positions.json", positionsJson);
}

std::pair<int, int> Gui::getPanelPositionForProcessor(const std::string& procName)
{
	if (positionsJson.contains(procName))
	{
		const auto procPos = positionsJson[procName];
		return std::pair<int, int>(procPos[0], procPos[1]);
	}
	else
	{
		return std::pair<int, int>(0, 0);
	}
}