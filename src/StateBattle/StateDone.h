/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _STATEDONE_H_
#define _STATEDONE_H_

#include <SDL/SDL.h>
#include "../Callback.h"
#include "../GameState.h"

class Menu;
class Player;

typedef enum {sbVictory, sbDefeat, sbNone} BattleResult;

class StateDone: public GameState {
public:
  // Constructor
  StateDone(Player *playerIn, Ship *enemy, BattleResult result);

  // Deconstructor
  ~StateDone();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // need to implement the switchFrom function
  void switchFrom();

  // Callbacks
  void exit(int ID);

private:
  // Menu class
  Menu *menu;

  int enemyGold;
  
  // Player object from state sailing, need to pass to StateBattle
  Player *player;

  // List of buttons, to find out whitch was called
  vector<string> ButtonList;

  TCallback<StateDone> exit_callback;
};

#endif
