/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _MAINMENU_H_ 
#define _MAINMENU_H_

#include <SDL/SDL.h>
#include <AL/al.h>
#include "GameState.h"
#include "Callback.h"
#include "Menu.h"

// So I don't have to include the headers
class StateSailing;

class MainMenu: public GameState {
public:
  // Constructor
  MainMenu(bool *doneIn);

  // Destructor
  ~MainMenu();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // Need to implement the switchFrom function
  void switchFrom();

  // Override the exit function
  void doExit();

  // Callback functions
  void continueGame(int i);
  void newGame(int i);
  void saveGame(int i);
  void loadGame(int i);
  void exit(int i);

private:
  // Set to false to exit
  bool *done;

  // Main sailing game state
  StateSailing *mainGame;

  // Menu class
  Menu *menu;

  // Callback functions
  TCallback<MainMenu> cont_callback;
  TCallback<MainMenu> new_callback;
  TCallback<MainMenu> save_callback;
  TCallback<MainMenu> load_callback;
  TCallback<MainMenu> exit_callback;
};

#endif
