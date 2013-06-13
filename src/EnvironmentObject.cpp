/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */
/*
 * This class is a parent to any object which may apear
 * in the game. Initialy the things that I can think of now
 * would be cities, hidden treasures, and maybe enemy ships.
 */

#include <string>
#include "ccmath.h"
#include "EnvironmentObject.h"

using namespace std;

EnvironmentObject::EnvironmentObject() {
  locX    = 0;
  locZ    = 0;
  visible = false;
  name    = "";
}

EnvironmentObject::~EnvironmentObject() {
}

EnvironmentObject::EnvironmentObject(string objName, int objLocX, int objLocZ, bool objVisible) {
  locX    = objLocX;
  locZ    = objLocZ;
  visible = objVisible;
  name    = objName;
} 

string EnvironmentObject::getName() {
  return name;
}

float EnvironmentObject::calcDist(float x, float z) {
  return sqrt((x-locX)+(x-locX)+(z-locZ)*(z-locZ));
}

float EnvironmentObject::calcAngle(float x, float z) {
  // Um, don't ask.
  float angle = atan2(locZ - z, x - locX);

  // atan2 returns from -PI to PI, so move 0 to 2*PI
  if(angle < 0) angle = 2*M_PI + angle;
  
  return angle;
}
