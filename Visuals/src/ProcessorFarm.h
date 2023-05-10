#pragma once

#include "ofMain.h"
#include "AbstractProcessor.h"

class ProcessorFarm {
public:
    ProcessorFarm() {};
    ~ProcessorFarm() {};

    void setup();
    void update();
    void draw();
    void addProcessor(AbstractProcessorPtr processor, const std::string& name);
    void setActiveProcessor(int index);
    const std::vector<AbstractProcessorPtr>& getProcessors();

private:
    std::vector<AbstractProcessorPtr> processors;
    ofParameterGroup params;
    std::vector<ofParameter<bool>> enabledParameters;
    int activeProcessorIndex = -1;
    void onEnabledParameterChanged(bool& value, int index);

    ofEventListeners enabledListeners;
};

typedef std::unique_ptr<ProcessorFarm> ProcessorFarmPtr;