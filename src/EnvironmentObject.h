/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _ENVIRONMENTOBJECT_H_
#define _ENVIRONMENTOBJECT_H_

#include <string>

using namespace std;

class EnvironmentObject {
public:
  // Construction is underway!
  EnvironmentObject();
  EnvironmentObject(string objName, int objLocX, int objLocZ, bool objVisible);
  
  // No idea why this is needed here but not in GameState
  virtual ~EnvironmentObject() = 0;

  // Returns the name of the object.
  string getName();

  // Returns distance from (x, z) to object
  float calcDist(float x, float z);

  // Return angle in radians from (x, z) to object
  float calcAngle(float x, float z);  

  virtual void DisplayObject() = 0;

protected:
  int    locX;
  int    locZ;
  bool   visible;
  string name;

private: // ?
};

#endif
