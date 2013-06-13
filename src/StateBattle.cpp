/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Config.h"
#include "Log.h"
#include "image.h"
#include "font.h"
#include "GameState.h"
#include "Menu.h"
#include "StateBattle/Battle.h"
#include "Player.h"
#include "StateBattle.h"

using namespace std;

StateBattle::StateBattle(Player *playerIn): GameState(NULL) {
  actualBattle = NULL;

  player = playerIn;
  // Stop the player, so when the leave they aren't moving
  player->speed = 0;

  exit_callback.SetCallback(this, &StateBattle::exit);
  attack_callback.SetCallback(this, &StateBattle::attack);

  menu = new Menu("./data/enterbattle.png", mainMenu, this);

  menu->addList("Title", 0, 0.2);
  menu->addList("Menu", 0, .4);

  menu->addDataItem("Ship Sighted!", "Title", 1, 1, 1);
                                                                                                
  menu->addMenuItem("Sail away", &exit_callback, "Menu", true);
  menu->addMenuItem("Attack", &attack_callback, "Menu", true);
}

StateBattle::~StateBattle() {
  delete menu;
  delete actualBattle;

  Log::logger->message("State Battle deleted");
}

void StateBattle::loop() {
  menu->loop();
} 

void StateBattle::exit(int ID) {
    switchState(previous, this);
}

void StateBattle::attack(int ID) {
  // Just to be on the safe side
  if(actualBattle != NULL) {
    delete actualBattle;
  }
  actualBattle = new Battle(player);
  switchState(actualBattle, previous);
}

void StateBattle::switchTo(GameState *oldState) {
  menu->switchTo(oldState);
}

void StateBattle::switchFrom() {
}
