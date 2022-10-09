#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.setSize(1024, 768);
	settings.setPosition(glm::vec2(550, 300));
	settings.resizable = true;
	settings.glVersionMajor = 2;
	settings.glVersionMinor = 1;
	settings.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.setSize(750, 750);
	settings.setPosition(glm::vec2(0, 300));
	settings.resizable = true;
	shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
	guiWindow->setVerticalSync(false);

	shared_ptr<ofApp> mainApp(new ofApp);
	mainApp->setupGui();
	ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);

	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
