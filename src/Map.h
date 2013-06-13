/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */


#ifndef _MAP_H_

#define _MAP_H_

#include <SDL/SDL.h>
#include "GameState.h"
// So I do not have to include the headers.
class StateSailing;

class Map: public GameState {
public:
  //Constructor
  Map(StateSailing *curGame, Player *playerPos);

  // Destructor
  virtual ~Map();

  // Need to implement the loop function
  void loop();

  void switchTo(GameState *oldState);
  
  void switchFrom();

  void doExit();

  // I plan on drawing the map in the upper left corner of the screen while sailing. let's see.
  void drawGui();

private:
  // Set to false to exit
  bool *done;
  
  // Background texture
  GLuint map_background;
 
  // Number of menu items
  //int numItems;
  
  // Selected menu item
  // int selected;
  
  // Handles processing SDL events
  void processEvents();

  // Main sailing game state
  StateSailing *mainGame;

  // Player, we will need to get our position on the map
  Player *PlayerMap;
 
  // Keyboard handler functions
  void handle_key_up(SDL_keysym* keysym);
  void handle_key_down(SDL_keysym* keysym);
  void handle_mouse_move();

  void display();

  int winWidth;
  int winHeight;

  int curMousePosX;
  int curMousePosY;
  
  bool transport;
  bool info;
};

#endif
