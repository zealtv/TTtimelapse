#include "ofApp.h"


//------------------------------------------------------------------------------
void ofApp::setup()
{
  ofLogToFile( "log.txt", true );
  ofSetFrameRate(30);
  
  settingsLoaded = false;
  if( xml.load("settings.xml") ) settingsLoaded = true;
  else ofLog(OF_LOG_ERROR, "SETTINGS NOT FOUND");
  
  exitHours = ofToInt( xml.getValue( "SCHEDULE:EXITHOURS", "0" ) ) ;
  exitMinutes = ofToInt( xml.getValue( "SCHEDULE:EXITMINUTES", "0" ) ) ;
  ofLog(OF_LOG_NOTICE, "Scheduled exit time: " + ofToString(exitHours) + ":" + ofToString(exitMinutes) );
  
  
  
  showLiveFeed1 = false;
  if( xml.getValue("DISPLAYS:DISPLAY1:LIVE", "false") == "true" )
  {
    showLiveFeed1 = true;
  }

  showLiveFeed2 = false;
  if( xml.getValue("DISPLAYS:DISPLAY2:LIVE", "false") == "true" )
  {
    showLiveFeed2 = true;
  }
  
  camName = xml.getValue("CAMERA:NAME", "unknownCamera" );
  outPath = xml.getValue("CAMERA:PATH", "~/Desktop/redundantCapturedFrames/" );
  ofLog(OF_LOG_NOTICE, "capture path: " + outPath );
  
  
  sequencePath1 = xml.getValue("DISPLAYS:DISPLAY1:PATH", "~/Desktop/redundantCapturedFrames/" );
  sequencePath2 = xml.getValue("DISPLAYS:DISPLAY2:PATH", "~/Desktop/redundantCapturedFrames/" );
  ofLog(OF_LOG_NOTICE, "sequence 1 path: " + sequencePath1 );
  ofLog(OF_LOG_NOTICE, "sequence 2 path: " + sequencePath2 );
  
  

  doSnapshot = false;
  if( xml.getValue("CAMERA:CAPTURE", "true") == "true" )  doSnapshot = true;
  
  
  doDateStamp = true;
  snapshotInterval = ofToInt( xml.getValue("CAMERA:INTERVAL", "60" ) ) * 1000;
  ofLog(OF_LOG_NOTICE, "snapshot interval: " + ofToString( snapshotInterval ) );
  
  
  myGrabber = IPVideoGrabber::makeShared();
  myGrabber->setUsername( xml.getValue("CAMERA:USERNAME", "admin" ) );
  myGrabber->setPassword( xml.getValue("CAMERA:PASSWORD", "" ) );
  myGrabber->setCameraName("D-Link");
  myGrabber->setURI( xml.getValue("CAMERA:SOURCE", "http://google.com" ) );
  myGrabber->setMaxReconnects( 99999 );
  myGrabber->connect();
  
  snapshotTimer = ofGetElapsedTimeMillis();

  
  fbo.allocate(1280, 720, GL_RGB);
  fbo.begin();
  ofClear( 255, 255, 255, 0 );
  fbo.end();


  
  ofDirectory outDir = ofDirectory( outPath );
  if( outDir.exists() )
  {
    outDir.allowExt("png");
    numFrames = outDir.listDir();
    ofLog( OF_LOG_NOTICE, ofToString(numFrames) + " files exist in out path");
  }
  else numFrames = 0;
  
  snapper.setup( &pixels, numFrames, outPath, camName, doDateStamp );

  
  int sequenceLength = ofToInt( xml.getValue( "DISPLAYS:SEQUENCELENGTH", "300" ) );
  ofLog(OF_LOG_NOTICE, "sequence length = " + ofToString( sequenceLength ) + " seconds" );
  
  //sequenceCheckInterval = ofToInt( xml.getValue( "DISPLAYS:SEQUENCEUPDATEINTERVAL", "15" ) );
  sequenceCheckInterval = 2 * sequenceLength;
  ofLog(OF_LOG_NOTICE, "sequence check interval = " + ofToString( sequenceCheckInterval ) + " seconds" );
  
  framerate = ofToInt( xml.getValue("DISPLAYS:SEQUENCEFRAMERATE", "15" ) );
  targetFrameTime = 1.0/(float)framerate;
  ofLog(OF_LOG_NOTICE, "target frametime = " + ofToString( targetFrameTime ) + " seconds" );
  
  totalFrameTime = 0;

  sequence1.initialize( sequencePath1, sequenceLength );
  sequence2.initialize( sequencePath2, sequenceLength );
  
}






//------------------------------------------------------------------------------
void ofApp::update()
{
  
  //this will shutdown at the specified time
  //if( ofGetHours() == exitHours && ofGetMinutes() == exitMinutes ) scheduledExit();
  
  //this will shutdown after the specified time
  if( ofGetMinutes() >= exitMinutes )
  {
    if( ofGetHours() >= exitHours )
    {
      scheduledExit();
    }
  }

  //can optimize here
  // if not displaying live feed, update and fbo drawing only need to be done before a snapshot
  //if( showLiveFeed1 || showLiveFeed2 )
  
  myGrabber->update();
  drawFrameToFbo();
  
  if( ofGetElapsedTimeMillis() - snapshotTimer > snapshotInterval && doSnapshot) takeSnapshot();
 
  if( sequence1.hasNewFrames ) sequence1.swapSequenceBuffers();
  if( sequence2.hasNewFrames ) sequence2.swapSequenceBuffers();
  
}






//------------------------------------------------------------------------------
void ofApp::drawFrameToFbo()
{
  fbo.begin();
  
  ofBackground(0,0,0);
  
  ofSetHexColor(0xffffff);
  
  int w = ofGetWidth();
  int h = ofGetHeight();
  
  float totalKbps = 0;
  float totalFPS = 0;
  
  ofSetColor(255,255,255,255);
  myGrabber->draw( 0, 0, 1280, 720); // draw the camera
  
  fbo.end();

}



//------------------------------------------------------------------------------
void ofApp::draw()
{
  if( showLiveFeed1 )
  {
    fbo.draw(0.0, 0.0, ofGetWidth() / 2.0, ofGetHeight() );
  }
  else
  {
    if( sequence1.getIsInitialized() ) sequence1.drawFrame( 0.0, 0.0, ofGetWidth()/2.0, ofGetHeight() );
  }

  
  if( showLiveFeed2 )
  {
    fbo.draw( ofGetWidth() / 2.0, 0, ofGetWidth()/2.0, ofGetHeight() );
  }
  else
  {
    if( sequence2.getIsInitialized() ) sequence2.drawFrame( ofGetWidth()/2.0, 0.0, ofGetWidth()/2.0, ofGetHeight() );
  }
  
  
  
  if( ofGetElapsedTimeMillis() - ( sequenceCheckTimer ) >  (float)sequenceCheckInterval * 1000.0 )
  {
    if( !showLiveFeed1 ) sequence1.getNewFrames();
    if( !showLiveFeed2 ) sequence2.getNewFrames();

    sequenceCheckTimer = ofGetElapsedTimeMillis();
  }
  
  
  totalFrameTime += ofGetLastFrameTime();

  if( totalFrameTime > targetFrameTime )
  {
    if( !showLiveFeed1 ) sequence1.getAdjustedFrame();
    if( !showLiveFeed2 ) sequence2.getAdjustedFrame();
    
    totalFrameTime = 0;
  }
  
  
  
}



//------------------------------------------------------------------------------
void ofApp::keyPressed(int key)
{
  if( key == 'r' )
  {
    sequence1.getNewFrames();
    sequence2.getNewFrames();
  }

  if( key == 'e' )
  {
    scheduledExit();
  }
  
}



//------------------------------------------------------------------------------
void ofApp::takeSnapshot()
{
  fbo.readToPixels( pixels );

  snapper.lock();
  snapper.startThread( true, false );
  snapper.unlock();
  
  snapshotTimer = ofGetElapsedTimeMillis();
}





//--------------------------------------------------------------
void ofApp::exit()
{
  ofLogWarning( "Exit called at " + ofGetTimestampString() );

}


//--------------------------------------------------------------
void ofApp::scheduledExit()
{
  ofLog( OF_LOG_WARNING, "scheduled exit called" );

  if( snapper.isThreadRunning() )
  {
    ofLog( OF_LOG_WARNING, "Waiting for snapshot thread to finish" );
    snapper.waitForThread();
  }
  
  if( sequence1.isThreadRunning() )
  {
    ofLog( OF_LOG_WARNING, "stopping sequence1 thread" );
    sequence1.stopThread();
  }

  if( sequence2.isThreadRunning() )
  {
    ofLog( OF_LOG_WARNING, "stopping sequence2 thread" );
    sequence2.stopThread();
  }
  
  ofExit();
}


