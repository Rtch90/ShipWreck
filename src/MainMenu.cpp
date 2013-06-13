/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdlib>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Config.h"
#include "image.h"
#include "Log.h"
#include "font.h"
#include "GameState.h"
#include "StateSailing.h"
#include "Menu.h"
#include "Callback.h"
#include "MainMenu.h"

MainMenu::MainMenu(bool *doneIn): GameState(NULL) {
  displaySplashScreen("./data/title.png", "", 1);
  // Set the static Gamestate mainMenu pointer to this
  GameState::setMainMenu(this);
  // Bool value to set to quit
  done = doneIn;

  // Set the pointer to the main sailing state to null
  mainGame = NULL;

  // Ahh.. this is better --Allanis
  cont_callback.SetCallback(this, &MainMenu::continueGame);
  new_callback.SetCallback(this, &MainMenu::newGame);
  save_callback.SetCallback(this, &MainMenu::saveGame);
  load_callback.SetCallback(this, &MainMenu::loadGame);
  exit_callback.SetCallback(this, &MainMenu::exit);

  menu = new Menu("./data/background.png", NULL, this);

  menu->addList("Menu", 0, .4);
  
  menu->addMenuItem("Continue Game", &cont_callback, "Menu", false);
  menu->addMenuItem("New Game", &new_callback, "Menu", true);
  menu->addMenuItem("Save Game", &save_callback, "Menu", false);
  menu->addMenuItem("Load Game", &load_callback, "Menu", true);
  menu->addMenuItem("Exit", &exit_callback, "Menu", true);

  /*
  if(!LoadOgg(".data/title.ogg", &buffer)) {
    Log::logger->message("Could not load data/title.ogg");
  }

  SetListener();
  
  source = GenSource(buffer);

  alSourcePlay(source);
  */
  Log::logger->message("MainMenu initialised");
}

MainMenu::~MainMenu() {

  if(previous == mainGame) previous = NULL;
  delete mainGame;
  delete menu;

  // Delete buffer and source
  /*
  alSourceStop(source);
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  */

  // Set the static GameState mainMenu back to NULL
  GameState::setMainMenu(NULL);

  Log::logger->message("MainMenu deleted");
}

void MainMenu::loop() {
  menu->loop();
}

void MainMenu::doExit() {
  *done = true;
}

void MainMenu::switchTo(GameState *oldState) {
  if(previous == NULL) {
    menu->changeEnabled("Continue Game", "Menu", false);
    menu->changeEnabled("Save Game", "Menu", false);
  } else {
    menu->changeEnabled("Continue Game", "Menu", true);
    menu->changeEnabled("Save Game", "Menu", true);
  }
    
  menu->switchTo(oldState);

  /*
  SetListener();
  alSourcePlay(source);
  */
}

void MainMenu::switchFrom() {
  //alSourceStop(source);
  //music.
}

void MainMenu::continueGame(int i) {
  if(previous != NULL) {
    switchState(previous, NULL);
  }
}
void MainMenu::newGame(int i) {
  // New game
  if(mainGame == NULL) {
    // Create a new game
    mainGame = new StateSailing();
  } else {
    // Just reset existing game
    mainGame->newGame();
  }
  switchState(mainGame, NULL);
}

void MainMenu::saveGame(int i) {
  // Save the game
  // Once I allow multiple save games this will need to change

  //Should not try to save if the game has not even started
  if(mainGame != NULL) {
    mainGame->saveGame();
    switchState(mainGame, NULL);
  }
}

void MainMenu::loadGame(int i) {
  if(mainGame == NULL) {
    // Need to initialize the gamestate
    mainGame = new StateSailing();
  }
  
  // Load the game
  mainGame->loadGame();
  
  // Now switch to game mode
  switchState(mainGame, NULL);
}

void MainMenu::exit(int ID) {
  doExit();
}
