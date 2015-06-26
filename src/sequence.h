#pragma once
#include "ofMain.h"

class sequence : public ofThread
{
public:
  void setPath( string path );
  void nextFrame();
  void updateSequence();
  void drawFrame( float x, float y, float w, float h );
  void initialize( string path );
  void initialize( string path, int slength );
  bool getIsInitialized();
  void getAdjustedFrame();
  void swapSequenceBuffers();
  void getNewFrames();
  bool hasNewFrames;
  
private:
  vector<ofFile> frames;
  vector<ofFile> newFrames;
  int sequenceSize;
  int sequenceIndex;
  ofImage sequenceFrame;
  string sequencePath;
  bool isInitialized;
  void threadedFunction();
  int sequenceLength;
  
};