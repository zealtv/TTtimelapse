#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "IPVideoGrabber.h"
#include "snapshot.h"
#include "sequence.h"

using ofx::Video::IPVideoGrabber;
using ofx::Video::SharedIPVideoGrabber;


class ofApp: public ofBaseApp
{
public:
  void setup();
  void update();
  void exit();
  void scheduledExit();
  void draw();
  void takeSnapshot();
  void drawFrameToFbo();
  void keyPressed(int key);
  
  
  int snapshotTimer;
  int snapshotInterval;
  int numFrames;
  bool doSnapshot;
  bool doDateStamp;
  
  int framerate;
  
  bool settingsLoaded;
  
  bool showLiveFeed1;
  bool showLiveFeed2;
  
  int exitHours;
  int exitMinutes;
  
  string capturePath;

  
  string camName;
  
  snapshot snapper;
  ofFbo fbo;
  ofPixels pixels;
  ofImage snapshot;  
  
  string outPath;
  
  SharedIPVideoGrabber myGrabber;
  
  ofxXmlSettings xml;
  
  
  sequence sequence1;
  sequence sequence2;
  
  int sequenceCheckInterval;
  int sequenceCheckTimer;
  
  float totalFrameTime;
  float targetFrameTime;
  string sequencePath1;
  string sequencePath2;
  
  
};
