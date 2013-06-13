/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _STATEBATTLE_H_
#define _STATEBATTLE_H_

#include <SDL/SDL.h>
#include "Callback.h"
#include "GameState.h"

class Menu;
class Battle;
class Player;

class StateBattle: public GameState {
public:
  //Constructor
  StateBattle(Player *playerIn);
  
  // Deconstructor
  ~StateBattle();
  
  // Implement the loop function
  void loop();

  // Implement the switchTo function
  void switchTo(GameState *oldState);

  // Implement the switchFrom function
  void switchFrom();

  // Callbacks
  void exit(int ID);
  void attack(int ID);

private:
  // Menu class
  Menu *menu;

  // Actual StateBattle
  Battle *actualBattle;

  // Player object from state sailing, need to pass to StateBattle
  Player *player;
  
  // List of buttons, to find out which was called
  vector<string> ButtonList;

  TCallback<StateBattle> exit_callback;
  TCallback<StateBattle> attack_callback;

};

#endif
