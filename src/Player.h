/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>

class Ship;
class Cargo;

using namespace std;

class Player {
public:
  Player();

  ~Player();
  
  // The ship contains all of these fields, we could just use them..
  GLfloat x;
  GLfloat z;
  GLfloat rot;
  GLfloat speed;
  
  bool load(int);
  bool save(int);
  void reset();

  void dump();

  // This just returns the ship's cargo, so you can avoid going through the ship
  Cargo* getCargo();
  
  // We will need to work this out once we get things like fleets and stuff.
  Ship *ship;

  int gold;
};

#endif
