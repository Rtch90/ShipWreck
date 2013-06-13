/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _GAMESTATE_H_ 
#define _GAMESTATE_H_

#include <string>

class GameState {
public:
  // Constructor
  GameState(GameState *prev);

  // Need a virtual destructor to make sure the proper destructor get's called.
  virtual ~GameState() = 0;

  // Initialize the mainMenu pointer
  // This only needs to happen once per execution
  static void setMainMenu(GameState *menu);

  // Initialize the currentPtr
  // This only needs to happen once per execution
  static void setCurrentPtr(GameState **current);

  // Main state loop
  virtual void loop() = 0;

  // Call this to exit
  virtual void doExit();

protected:
  GameState *previous;
  
  // All states share these pointers, so make them static
  static GameState *mainMenu;
  static GameState **currentPtr;

  // For screenshots
  static unsigned char nextScreen;

  // Save a screenshot to disk
  void takeScreenshot();

  // Display a splash screen
  void displaySplashScreen(std::string imageFile, std::string text, float shade);

  // Switch to next, giving prev as the previous state
  virtual void switchState(GameState *next, GameState *prev);

private:
  // Things to do when switching to this state from oldState
  virtual void switchTo(GameState *oldState) = 0;

  // Things to do when switching from this state
  virtual void switchFrom() = 0;
};

#endif
