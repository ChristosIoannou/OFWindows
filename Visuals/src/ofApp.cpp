#include "ofApp.h"
#include "VideoPlayerProcessor.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	farm = std::make_unique<ProcessorFarm>();
	gui = std::make_unique<Gui>();

	auto videoPlayerProcessor = std::make_shared<VideoPlayerProcessor>();
	farm->addProcessor(videoPlayerProcessor, "Video Player");

	
	gui->generatePanels(*farm);
}

//--------------------------------------------------------------
void ofApp::update()
{
	farm->update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	farm->draw();
}

//--------------------------------------------------------------
void ofApp::exit() 
{

}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	gui->setup();
}

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs& args)
{
	gui->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
