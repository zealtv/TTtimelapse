#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main()
{
  //ofSetLogLevel(OF_LOG_ERROR);
  //ofSetLogLevel(OF_LOG_VERBOSE);
  ofSetLogLevel(OF_LOG_NOTICE);
  
  ofAppGLFWWindow window;
  window.setMultiDisplayFullscreen(true);
  ofSetupOpenGL( &window, 1280 * 2, 720, OF_FULLSCREEN );
  ofRunApp(new ofApp());
}
