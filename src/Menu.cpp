/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 *
 * This class is intended to be a base class for all the menu systems
 * in Lephisto. this will extend the GameState class.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdlib>
#include <vector>
#include <string>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Config.h"
#include "image.h"
#include "font.h"
#include "GameState.h"
#include "Callback.h"
#include "Log.h"
#include "Menu.h"

using namespace std;

Menu::Menu(const char* image, GameState *from, GameState *to): GameState(NULL) {
  // Bool valie to set to quit
  done = false;

  // Make Set from and to gamestates
  fromState = from;
  toState   = to;

  // Get ready for text mode
  EnterTextMode(Config::configuration->getWinWidth(),
                Config::configuration->getWinHeight());

  // Load the background
  glEnable(GL_TEXTURE_2D);
  BuildTexture(image, &background, GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, background);

  Selected = 0;
  ButtonID = 0;
  mouseActive = true;
}

Menu::~Menu() {
  unsigned int i;
  glDeleteTextures(1, &background);

  for(i = 0; i < lists.size(); i++) {
    delete lists[i];
  }

  lists.clear();
}

void Menu::loop() {
  while(!lists[Selected]->Select()) {
    Selected += 1;
    if(Selected > (int) lists.size()) Selected = 0;
  }

  // Draw the menu
  display();

  // Process incoming events
  processEvents();
}

void Menu::display() {
  int width  = Config::configuration->getWinWidth();
  int height = Config::configuration->getWinHeight();
  int i;

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  // Draw the background
  // Bind the background texture
  glBindTexture(GL_TEXTURE_2D, background);

  // Set the color
  glColor3f(.8, .8, .8);

  // Draw the actual background quad
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0 , 0);
  glTexCoord2f(0, 1);
  glVertex3f(0, height, 0);
  glTexCoord2f(1, 1);
  glVertex3f(width, height, 0);
  glTexCoord2f(1, 0);
  glVertex3f(width, 0, 0);
  glEnd();

  // Draw the text.
  for(i = 0; i < (signed)lists.size();  i++) {
    glLoadIdentity();
    //glTranslatef((float) width * lists[i].x, (float) height * lists[i].y, 0);
    lists[i]->Display();
  }
  glColor3f(1, 1, 1);
  
  SDL_GL_SwapBuffers();
}

void Menu::processEvents() {
  // Our SDL event placeholder 
  SDL_Event event;

  handle_mouse();

  // Grab all the events off the queue
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      // Handle key presses
      handle_key_down(&event.key.keysym);
      break;
    case SDL_KEYUP:
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      // Handle quit requests (like Ctrl-c)
      doExit();
      break;
    }
  }
}

void Menu::handle_key_up(SDL_keysym* keysym) {
  switch(keysym->sym) {
  case SDLK_ESCAPE:
    // Go back to the game, if it is loaded anyway.
    if((fromState != NULL) && (toState != NULL)) {
      switchState(fromState, toState);
    }
    break;
  case SDLK_RETURN:
    lists[Selected]->Execute();
    break;
  case SDLK_p:
    Print();
    break;
  default:
    break;
  }
}

void Menu::handle_key_down(SDL_keysym* keysym) {
  mouseActive = false;
  int newSelected;

  switch(keysym->sym) {
  case SDLK_UP:
    lists[Selected]->SelectionUp();
    break;
  case SDLK_DOWN:
    lists[Selected]->SelectionDown();
    break;
  case SDLK_LEFT:
    newSelected = Selected - 1;
    if(newSelected < 0) newSelected = lists.size() - 1;
    while(!lists[newSelected]->Select()) {
      newSelected -= 1;
      if(newSelected < 0) newSelected = lists.size() - 1;
    }
    lists[Selected]->Deselect();
    Selected = newSelected;
    break;
  case SDLK_RIGHT:
    newSelected = Selected + 1;
    if(newSelected >= (int) lists.size()) newSelected = 0;
    while(!lists[newSelected]->Select()) {
      newSelected += 1;
      if(newSelected >= (int) lists.size()) newSelected = 0;
    }
    lists[Selected]->Deselect();
    Selected = newSelected;
    break;
  case SDLK_z:
    takeScreenshot();
    break;
  default:
    break;
  }
  
}

void Menu::doExit() {
  *done = true;
}

void Menu::switchTo(GameState *oldState) {
  // Set selected to return to game
  Selected = 0;

  while(!lists[Selected]->Select()) {
    Selected += 1;
    if(Selected > (int) lists.size()) Selected = 0;
  }

  EnterTextMode(Config::configuration->getWinWidth(),
                Config::configuration->getWinHeight());

  glEnable(GL_TEXTURE_2D);
}

void Menu::switchFrom() {
  lists[Selected]->Deselect();
  Selected = -1;
}

void Menu::addList(string Name, float x, float y) {
  MenuContainer *tempContainer;
  
  tempContainer = new MenuContainer(Name, x, y, maLeft);

  lists.push_back(tempContainer);
}
 
void Menu::addList(string Name, float x, float y, int alignment) {
  MenuContainer *tempContainer;
  
  if((alignment < maLeft) || (alignment > maRight))
    addList(Name, x, y);

    tempContainer = new MenuContainer(Name, x, y, alignment); 
  lists.push_back(tempContainer);
}

int Menu::addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled) {
  unsigned int i;
  
  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->GetName() == List) {
      lists[i]->AddButton(Text, recallFunc, enabled, ButtonID);
      ButtonID++; 
      break;
    }
  }

  return ButtonID - 1;
}

int Menu::addMenuItem(string Text, cCallback *recallFunc, string List, bool enabled, float red, float green, float blue) {
  unsigned int i;

  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->GetName() == List) {
      lists[i]->AddButton(Text, recallFunc, enabled, ButtonID, red, green, blue);
      ButtonID++;
      break;
    }
  }

  return ButtonID - 1;
}

void Menu::addDataItem(string Text, string List) {
  unsigned int i;

  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->GetName() == List) {
      lists[i]->AddData(Text);
      break;
    }
  }
}

void Menu::addDataItem(string Text, string List, float red, float green, float blue) {
  unsigned int i;

  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->GetName() == List) {
      lists[i]->AddData(Text, red, green, blue);
      break;
    }
  }
}

void Menu::changeEnabled(string Text, string List, bool enabled) {
  unsigned int i;

  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->GetName() == List) {
      lists[i]->ChangeEnabled(Text, enabled);
    }
  }
}

void Menu::handle_mouse() {
  unsigned int i;
  Uint8 buttons;

  buttons = SDL_GetMouseState(&curMousePosX, &curMousePosY);
  
  if((!mouseActive) && (!((curMousePosX == lastMousePosX) && (curMousePosY == lastMousePosY))))
    mouseActive = true;

  buttonDown = MouseButtonDown(buttons, 1);

  for(i = 0; i < lists.size(); i++) {
    if(lists[i]->LocationWithin(curMousePosX, curMousePosY)) {
      if(lists[i]->MouseSelect(curMousePosX, curMousePosY)) {
        if(Selected != (int) i) {
          lists[Selected]->Deselect();
	  Selected = i;
        }
	if(mouseActive && buttonDown) {
	  lists[Selected]->Execute();
        }
      }
    }
  }

  lastMousePosX = curMousePosX;
  lastMousePosY = curMousePosY;
}

bool Menu::MouseLocToMenu(int locX, int locY) {
/*
  unsigned int i;
  
  for(i = 0; i < lists.size(); i++) {
    if((locX >= lists[i].TopLeftX)
      && (locX <= lists[i].BottomRightX)
      && (locY >= lists[i].TopLeftY)
      && (locY <= lists[i].BottomRightY)) {
      
      hSelected = (int) i;
      return ContainerSelection(locX, locY, i);
    }
  }
  
  return false;
*/
  return false;
}

bool Menu::ContainerSelection(int locX, int locY, int container) {
/*
  unsigned int i;
  float vertical;
  menuItem* tempItem;

  vertical = lists[container].TopLeftY;
  //tempItem = lists[container];

  for(i = 0; i < lists[container].menuItems.size(); i++) {
    tempItem = &lists[container].menuItems[i];
    if((locY > (vertical + (18 * tempItem->order))
      && (locY < (vertical + (18 * (tempItem->order + 1)))))) {

      vSelected = (int) i;
      return true;
    }
  }
*/
  return false;
}

void Menu::Print() {
  unsigned int i;
  
  for(i = 0; i < lists.size(); i++)
    lists[i]->Print();
}

bool Menu::MouseButtonDown(Uint8 buttons, int buttonNumber) {
  if(((buttons & buttonNumber) == buttonNumber) && ((lastButtonState & buttonNumber) == buttonNumber)) {
    lastButtonState = buttons;
    return false;
  }

  if(((buttons & buttonNumber) == buttonNumber) && ((lastButtonState & buttonNumber) != buttonNumber)) {
    lastButtonState = buttons;
    return true;
  }

  lastButtonState = buttons;
  return false;
} 
