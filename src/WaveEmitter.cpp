/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <GL/gl.h>
#include <vector>
#include <list>
#include <time.h>
#include <stdlib.h>
#include "Log.h"
#include "image.h"
#include "Point.h"
#include "Terrain.h"
#include "BoundBox.h"
#include "Wave.h"
#include "WaveEmitter.h"

using namespace std;

#define WAVE_RATE .002
#define EMIT_RATE 3300
#define EMIT_DIFF_PERCENT .2
#define EMIT_RATIO .2

class WaveLoc {
public:
  float x;
  float z;
  float angle;
};

GLuint WaveEmitter::waveList = 0;
list <Wave*> WaveEmitter::activeWaves;

WaveEmitter::WaveEmitter(Terrain *terrain, BoundBox *box) {
  // List of points on shore
  list< Point* > shorePoints;
  Point *temp;
  float tAngle = 0.0;
  WaveLoc *tLoc;
 
  ticksToNextEmit = (int) (EMIT_RATE*(rand()/(RAND_MAX+1.0)));

  //counter = 0.0;

  isVisible = false;

  // Populate the shore line list
  terrain->locateShoreLine(box, &shorePoints);

  while(!shorePoints.empty()) {
    // First, we need to read the front element of the list
    temp = shorePoints.front();
    // Next, remove the shore element
    shorePoints.pop_front();
    
    // Find shore angle and move point to propper distance
    if(terrain->findNearestShoreAngle(temp, &tAngle)) {
      // Set tLoc angle and move point to propper distance
      tLoc        = new WaveLoc();
      tLoc->x     = temp->x;
      tLoc->z     = temp->z;
      tLoc->angle = tAngle;
      waveLocations.push_back(tLoc);
    }

    // Finaly delete the Point the list element pointed to
    delete temp;
  }
}

WaveEmitter::~WaveEmitter() {
  // Delete the wave locations in vector
  WaveLoc *temp;
  while(!waveLocations.empty()) {
    temp = waveLocations.back();
    waveLocations.pop_back();
    delete temp;
  }
}

void WaveEmitter::update(unsigned int ticks) {
  int numToGenerate;
  
  ticksToNextEmit -= ticks;
 
  if(ticksToNextEmit <= 0) {
    // Update ticksToNextEmit
    // I add EMIT_RATE and ticksToNextEmit so that it keeps the emit rate exact (I hope)
    ticksToNextEmit = getNextEmit() + ticksToNextEmit;

    // Add a new wave (only if it is visible???)
    if(isVisible) {
      numToGenerate = 1+(int) (waveLocations.size() * EMIT_RATIO * (rand()/(RAND_MAX+1.0)));
      for(int i = 0; i < numToGenerate; i++) {
        generateWave();
      }
    }
  }
  
  isVisible = false;
}

void WaveEmitter::setVisible() {
  isVisible = true;
}

int WaveEmitter::getNextEmit() {
  int diff = (int) (EMIT_DIFF_PERCENT * EMIT_RATE);
  float randNum = (2.0 * (rand() / (RAND_MAX + 1.0))) - 1;

  return EMIT_RATE + (int) (diff * randNum);
}

void WaveEmitter::generateWaveList() {
  // Generate the surf list
  waveList = glGenLists(1);
  if(waveList == 0) {
    // Should throw an exception
    return;
  }
 
  glNewList(waveList, GL_COMPILE);

  // Draw the actual surf
  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex3f(1, 0, -.3);
  glTexCoord2f(1, 1);
  glVertex3f(-1, 0, -.3);
  glTexCoord2f(1, 0);
  glVertex3f(-1, 0, .3);
  glTexCoord2f(0, 0);
  glVertex3f(1, 0, .3);
  glEnd();

  glEndList();
}

void WaveEmitter::deleteWaveList() {
  glDeleteLists(waveList, 1);
}

void WaveEmitter::generateWave() {
  WaveLoc *temp;
  int index;

  index = (int) (waveLocations.size() * (rand() / (RAND_MAX + 1.0)));

  temp = waveLocations[index];

  activeWaves.push_back(new Wave(temp->x, temp->z, temp->angle));
}

int WaveEmitter::numWaveLocations() {
  return waveLocations.size();
}

void WaveEmitter::drawActiveWaves() {
  Wave *temp;
  list< Wave* >::iterator iter;
  list< Wave* >::iterator listEnd;
  
  if(activeWaves.size() <= 0) return;

  glEnable(GL_COLOR_MATERIAL);
  // Disable depth writes
  glDepthMask(GL_FALSE);

  iter = activeWaves.begin();
  listEnd = activeWaves.end();
  while(iter != listEnd) {
    temp = *iter;
    glPushMatrix();
    temp->prepDraw();
    glCallList(waveList);
    glPopMatrix();
    
    ++iter;
  }

  // Reenable depth testing
  glDepthMask(GL_TRUE);
  glDisable(GL_COLOR_MATERIAL);
}

void WaveEmitter::updateActiveWaves(unsigned int ticks) {
  Wave *temp;
  list< Wave* >::iterator iter;
  list< Wave* >::iterator listEnd;
  
  if(activeWaves.size() <= 0) return;

  temp = activeWaves.front();
  while(!activeWaves.empty() && !temp->isActive()) {
    activeWaves.pop_front();
  }
  
  iter = activeWaves.begin();
  listEnd = activeWaves.end();
  while(iter != listEnd) {
    temp = *iter;
    temp->update(ticks);

    ++iter;
  }
}

void WaveEmitter::deleteActiveWaves() {
  Wave *temp;
      
  while(!activeWaves.empty()) {
    temp = activeWaves.front();
    activeWaves.pop_front();
    delete temp;
  }
}
