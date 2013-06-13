/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _SAILINGSTATE_H_ 
#define _SAILINGSTATE_H_

#include <SDL/SDL.h>

// So I don't have to include the headers
class GameState;
class Player;
class SkyBox;
class Terrain;
class Ocean;
class Camera;
class Map;
class CityManager;
class Economy;
class StateCity;
class Ship;

class StateSailing: public GameState {
public:
  // Constructor
  StateSailing();
  
  // Decontructor
  ~StateSailing();

  // Reset the game, used when "New Game" is chosen from the menu
  void newGame();

  // Save the game, used when "Save Game" is chosen from the menu
  void saveGame();

  // Load the game, used when "Load Game" is chosen from the menu
  void loadGame();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // Need to implement the switchFrom function
  void switchFrom();

  // City Manager needs to be public for a map call.
  CityManager* cities;

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

  void setTicksToNextBattle();

  // To keep track of time
  unsigned int lastTicks;

  int ticksToNextBattle;

  // Stores left and right button states
  bool keys[2];

  // For sway of ship
  GLfloat rotx;

  // Player data
  Player *player;

  // SkyBox object
  SkyBox *skybox;

  // Terrain object
  Terrain *land;

  // Map object
  Map *map;

  // Economy
  //Economy* economy;

  // City state
  GameState *childState;

  // Camera location
  Camera *camera;
  GLfloat cameraX, cameraY, cameraZ;

  // Light info
  GLfloat ambientLight[4];
  GLfloat diffuseLight[4];
  GLfloat lightPosition[4];
};

#endif
