#include "sequence.h"

//------------------------------------------------------------------------------
void sequence::initialize( string path )
{
  isInitialized = false;
  setPath( path );
  updateSequence();
  isInitialized = true;
  sequenceLength = 300;
}

void sequence::initialize( string path, int slength )
{
  isInitialized = false;
  setPath( path );
  updateSequence();
  isInitialized = true;
  sequenceLength = slength;
}

//------------------------------------------------------------------------------
void sequence::setPath( string path )
{
  sequencePath = path;
}



//------------------------------------------------------------------------------
void sequence::nextFrame()
{
  sequenceIndex += 1;
  
  if( sequenceIndex >= sequenceSize )
  {
   sequenceIndex = 0;
  }
  
  sequenceFrame.loadImage( frames[ sequenceIndex ] );
}



//------------------------------------------------------------------------------
void sequence::getAdjustedFrame()
{
  
  float playhead = ofGetElapsedTimeMillis() % (sequenceLength * 1000);
  playhead = ofMap( playhead, 0.0, (sequenceLength * 1000), 0.0, sequenceSize );
  
  sequenceIndex = (int)playhead;
  
  if(sequenceSize > 0) sequenceFrame.loadImage( frames[ sequenceIndex ] );
}



//------------------------------------------------------------------------------
void sequence::updateSequence()
{
  ofDirectory dir( sequencePath );
  
  //only show png files
  dir.allowExt("png");
  
  //populate the directory object
  dir.listDir();
  dir.sort();
  
  frames = dir.getFiles();
  
  sequenceSize = frames.size();
  sequenceIndex = 0;
  
  if(sequenceSize > 0) sequenceFrame.loadImage( frames[ sequenceIndex ] );
  
  hasNewFrames = false;
}



//------------------------------------------------------------------------------
void sequence::drawFrame(float x, float y, float w, float h )
{
  if( sequenceFrame.isAllocated() ) sequenceFrame.draw( x, y, w, h );
}



//------------------------------------------------------------------------------
bool sequence::getIsInitialized()
{
  return isInitialized;
}



//------------------------------------------------------------------------------
void sequence::getNewFrames()
{
  lock();
  startThread( true, false );
  unlock();
}



//------------------------------------------------------------------------------
void sequence::threadedFunction()
{
  ofDirectory dir( sequencePath );
  
  //only show png files
  dir.allowExt("png");
  
  //populate the directory object
  dir.listDir();
  
  dir.sort();
  
  newFrames = dir.getFiles();
  
  if(newFrames.size() != frames.size() )
  {
    if(newFrames.size() > frames.size() ) ofLog( OF_LOG_NOTICE, "New frames found" );
    if(newFrames.size() < frames.size() ) ofLog( OF_LOG_WARNING, "FRAMES LOST!" );

    hasNewFrames = true;
  }
  else ofLog( OF_LOG_NOTICE, "No new frames found" );

  stopThread();
}



//------------------------------------------------------------------------------
void sequence::swapSequenceBuffers()
{
  
  ofLog( OF_LOG_NOTICE, "SWAPPING BUFFERS" );
  frames = newFrames;
  sequenceSize = frames.size();
//  sequenceIndex = 0;
  sequenceIndex %= sequenceSize;
  hasNewFrames = false;
}
