/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 *
 * The Menu Class is used as out basis for any of the menus in the game.
 * It has functionality to create lists of buttons or to display data
 * on top of a background image. There are a number of improvements
 * which could be made to the Menu class (animation, better mouse control,
 * changes in font, color, different kinds of input like a text box
 * control, etc.) but at the moment, this is simple and functional.
 *
 *##############################################################################
 *  This is just a brief overview of the functionality of the new menu class.
 * ---------------------------------------------------------------------------
 * For a start, it is a child class of GameState. So it inherits all the
 * functionality of the parent. Usualy you aren't going to give it full
 * control of execution. You'll have another GameState call menu->loop()
 * inside its own loop(). This also means that you need to call menu->switchTo()
 * in your other GameStates switchTo() class, because there is some important
 * display code that won't load if you don't. (Look at the way MainMenu.cpp is set up
 * if you are confused.
 *
 * The constructor is typical, it takes a background image as an argument. It 
 * is then ready to be displayed with loop().
 *
 * The menu functionality of the class is found in two structures: Lists and
 * items... If you think about how a menu is displayed, there are different
 * options one on top of the next. So the idea of a lists is that it is the
 * container for those options. Having this structure allows you to have different
 * lists in different parts of the screen.
 *
 * To create a list, you'll use a Menu::addList(string Name, float x, float y).
 * The name is realy important. It is how you get back to the list you have
 * created. The two float arguments are the location on the location on the screen. The location
 * is a percentage of the screen, so you're limited to between 1 and 0. It won't
 * error check this because it just means you won't be able to see the list.
 *
 * To add items to the list, there are two different items you have access to.
 * MenuItems are selectable. You need to provide the text to display, a callback
 * function, which list you want it to be part of and whether or not it's
 * selectable. You can change if it's selectable or not. Each MenuItem in a menu
 * gets a unique ID which is returned when you call Menu::addMenuItem. This can
 * be used to identify which button was pressed, or used to modify the buttons
 * if needed. The recall function is is relatively complex, so you'll need to
 * look at either MainMenu or StateCity to see it in use.
 * The recall functions are void and takes one argument (int).
 * This argument is the ID of the button which was pressed, so that mutiple
 * buttons can use the same recall function and still distinguish between which
 * called the recall function. At this point, there is no other kind of
 * function allowed.
 *
 * The other kind of item is a data item. This needs no callback function because
 * it can never be selected. It's handy when you need to just display information
 * (say for example the economy in a city).
 *
 * Menu::changeEnabled(string Text, string List, bool enabled) moves a menu item
 * from being enabled or not depending on if you pass false or true to the final
 * argument.
 *
 * Menu::switchToData and Menu::switchToMenu are not fully tested and probably don't work.
 * Please don't use them.
 *#######################################################################################
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <vector>
#include <string>
#include "SDL/SDL.h"
#include "GameState.h"
#include "Callback.h"
#include "StateSailing.h"
#include "Menu/MenuContainer.h"

using namespace std;

typedef enum {maLeft, maCenter, maRight} MenuAlignment;

class Menu: public GameState {
public:
  // Constructor
  Menu(const char* image, GameState *from, GameState *to);

  // Destructor
  ~Menu();

  // Need to implement the loop function
  void loop();

  // Need to implement the switchTo function
  void switchTo(GameState *oldState);

  // Need to implement the switchFrom function
  void switchFrom();
  
  // Override the exit function
  void doExit();

  // Add a list
  void addList(string Name, float x, float y);
  void addList(string Name, float x, float y, int alignment);

  // Add a menu item
  int addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled);
  //int addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled, GLuint image);
  int addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled, float red, float green, float blue);
  //int addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled, GLuint image,
		    //float red, float green, float blue);

  // Add a data item
  void addDataItem(string Text, string List); // Doesn't need a callback because it is not selectable
  //void addDataItem(string Text, string List, GLuint image);
  void addDataItem(string Text, string List, float red, float green, float blue);

  // Enable/Disable a menu Item
  void changeEnabled(string Text, string List, bool enabled);

private:
  void Print();

  // Set to false to exit
  bool *done;

  // Background texture
  GLuint background;

  // Menu lists
  vector<MenuContainer*> lists;
  
  // Selected list item
  int Selected;

  // Handles processing SDL events
  void processEvents();
  
  // Keyboard handler functions
  void handle_key_up(SDL_keysym* keysym);
  void handle_key_down(SDL_keysym* keysym);

  // Mouse functions
  void handle_mouse();

  /*
   * MouseButtonDown takes a button number
   * (Left = 1, Middle = 2, Right = 3),
   * and the current butons pressed, it
   * tells you true if this is the first frame
   * in which the button was pressed. So it
   * will not return true every frame the button
   * is pressed, but only the first time for
   * each click.
   * to find out if the buttin is pressed every fram try:
   * buttonDown = (buttons == SDL_BUTTON(buttonNumber));
   */
  bool MouseButtonDown(Uint8 buttons, int buttonNumber);
  Uint8 lastButtonState;

  /*
   * MouseLocToMenu translates the location
   * of the mouse to the corresponding button
   * on the menu. It returns true if it is
   * within a button, and false otherwise.
   * It also changes the hSelected if it needs to.
   */
  bool MouseLocToMenu(int locX, int locY);

  /*
   * This is similar to MoseLocToMenu exept
   * its at the listContainer level and it will
   * change vSelected.
   */
  bool ContainerSelection(int locX, int locY, int container);

  // Mouse variables
  int curMousePosX;
  int curMousePosY;
  int lastMousePosX;
  int lastMousePosY;
  bool buttonDown;
  bool mouseActive;

  // Gamestates for switching
  GameState *fromState;
  GameState *toState;

  void display();

  /*
   * ButtonID is a unique identifier for each button,
   * it can be used instead of the name of the button,
   * and the name of the list together. It is also used
   * in callback situations.
   */
  int ButtonID;
};

#endif
