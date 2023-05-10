#include "ProcessorFarm.h"

void ProcessorFarm::setup() {
    // Set up each processor
    for (auto& processor : processors) {
        processor->setupProcessor();
    }
}

void ProcessorFarm::update() {
    if (activeProcessorIndex >= 0 && activeProcessorIndex < processors.size()) {
        processors[activeProcessorIndex]->update();
    }
}

void ProcessorFarm::draw() {
    if (activeProcessorIndex >= 0 && activeProcessorIndex < processors.size()) {
        processors[activeProcessorIndex]->draw();
    }
}

void ProcessorFarm::addProcessor(AbstractProcessorPtr processor, const std::string& name) {

    if (!processor) {
        std::cerr << "Error: processor is null" << std::endl;
        return;
    }

    processor->setupProcessor();
    processors.emplace_back(processor);

    ofParameter<bool> enabledParam;
    enabledParam.set(name, false);
    enabledParameters.push_back(enabledParam);
    processor->params.add(enabledParameters.back());

    int index = processors.size() - 1;
    enabledListeners.push(enabledParameters[index].newListener([this, index](bool& value) {
        this->onEnabledParameterChanged(value, index);
        })
    );
}

void ProcessorFarm::setActiveProcessor(int index) {
    if (index >= 0 && index < processors.size()) {
        if (activeProcessorIndex != -1) {
            // Stop the previous processor
            enabledParameters[activeProcessorIndex] = false;
        }
        activeProcessorIndex = index;
    }
}

void ProcessorFarm::onEnabledParameterChanged(bool& value, int index) {
    if (value) {
        setActiveProcessor(index);
    }
    else {
        activeProcessorIndex = -1;
    }
}

const std::vector<AbstractProcessorPtr>& ProcessorFarm::getProcessors()
{
    return processors;
}