/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <list>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../GameState.h"
#include "StateDone.h"

using namespace std;

// So I don't have to include the headers
class Player;
class Camera;
class Ship;
class Bullet;
class ShipAI;
class StateDone;

class Battle: public GameState {
 public:
  // Constructor
  Battle(Player *playerIn);
  
  // Decontructor
  ~Battle();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // Need to implement the switchFrom function
  void switchFrom();

 private:
  // Handles processing SDL events
  void processEvents();

  // Keyboard handler functions
  void handle_key_up(SDL_keysym* keysym);
  void handle_key_down(SDL_keysym* keysym);

  // Timer function to handle time-based movement
  void timer();

  // Function to display actual scene
  void display();

  // Fire cannons
  void playerFire(float rotDiff);

  void enemyFire(float rotDiff);
  
  // Record a collision
  void playerHit();
  void enemyHit();
  
  void battleFinished(BattleResult result);

  // To keep track of time
  unsigned int lastTicks;

  // Stores left and right button states
  bool keys[2];

  // Ship model
  Ship *ship;
  Ship *enemyShip;

  // Object to hand to AI for enemy ship
  ShipAI *enemyAI;

  // For sway of ship
  GLfloat rotx;

  // Player data
  Player *player;

  int playerTicksToReload;

  int enemyTicksToReload;

  // List of cannon balls in the air
  list <Bullet*> bullets;

  // Camera location
  Camera *camera;
  GLfloat cameraX, cameraY, cameraZ;
  
  // State to go to after battle is done
  StateDone *leaveState;
  
  // Light info
  GLfloat ambientLight[4];
  GLfloat diffuseLight[4];
  GLfloat lightPosition[4];

  GLfloat waterMaterial[4];
  GLfloat bulletMaterial[4];
  
  GLuint waterTexture;
};

#endif
