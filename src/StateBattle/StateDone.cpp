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
#include "../Config.h"
#include "../Log.h"
#include "../image.h"
#include "../font.h"
#include "../GameState.h"
#include "../ccmath.h"
#include "../Menu.h"
#include "../Player.h"
#include "../Ships.h"
#include "../Cargo.h"
#include "StateDone.h"

using namespace std;

StateDone::StateDone(Player *playerIn, Ship *enemy, BattleResult result) : GameState(NULL) {
  enemyGold = (int) (randBellCurve() * 100.0);
  enemy->cargo->fillRandom();
  enemy->cargo->dump();

  player = playerIn;

  exit_callback.SetCallback(this, &StateDone::exit);

  menu = new Menu("data/enterbattle.png", mainMenu, this);

  menu->addList("Title", 0, 0.2);
  menu->addList("Menu", 0, .4);

  switch(result) {
  case sbVictory:
    menu->addDataItem("Victory!", "Title", 1, 1, 0);
    menu->addDataItem("You get the enemy's gold.", "Title", 1, 1, 0);

    player->gold += enemyGold;
    break;
  case sbDefeat:
    menu->addDataItem("You were defeated!", "Title", 1, 0, 0);
    menu->addDataItem("The enemy wrecks your ship and steals all of your gold.", "Title", 1, 1, 0);
    player->ship->damage = 1;
    player->gold = 0; // Poor you
    break;
  case sbNone:
    menu->addDataItem("The battle draws to a close as your ships slip apart.", "Title", 1, .5, 0);
    break;
  }

  menu->addMenuItem("Done", &exit_callback, "Menu", true);
}

StateDone::~StateDone() {
  delete menu;
  Log::logger->message("State Done deleted");
}

void StateDone::loop() {
  menu->loop();
}

void StateDone::exit(int ID) {
  switchState(previous, this);
}

void StateDone::switchTo(GameState *oldState) {
  menu->switchTo(oldState);
}

void StateDone::switchFrom() {
}
