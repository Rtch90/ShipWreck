/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _STATECITY_H_
#define _STATECITY_H_

#include "SDL/SDL.h"
#include "Callback.h"
#include "GameState.h"

class StateSailing;
class City;
class Player;
class Menu;

class StateCity: public GameState {

public:
  // Constructor
  StateCity(City *townIn, Player *data);

  // destructor
  ~StateCity();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // Need to impement the switchFrom function
  void switchFrom();

  // Callbacks
  void exit(int ID);
  void buy(int ID);
  void sell(int ID);
  
private:
  // Point to current City object
  City *town;
  // Menu class
  Menu *menu;

  // Player class
  Player *PlayerInfo;

  // List of buttons, to find out which was called
  vector<string> ButtonList;

  vector<Product*> productList;

  TCallback<StateCity> exit_callback;
  TCallback<StateCity> buy_callback;
  TCallback<StateCity> sell_callback;
};

#endif
