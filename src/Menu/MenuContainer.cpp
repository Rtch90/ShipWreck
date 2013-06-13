/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../font.h"
#include "../Callback.h"
#include "../Config.h"
#include "../Log.h"
#include "../Menu.h"
#include "DataContainer.h"
#include "Button.h"
#include "MenuContainer.h"

using namespace std;

MenuContainer::MenuContainer(string name, float x, float y, int alignment) {
  myName = name;
  locX = x;
  locY = y;

  TopLeftX = (float)Config::configuration->getWinWidth() * x;
  TopLeftY = (float)Config::configuration->getWinHeight() * y;
  BottomRightX = TopLeftX;
  BottomRightY = TopLeftY;

  align = alignment;

  selected = false;
  button = -1;
}

MenuContainer::~MenuContainer() {
  unsigned int i;

  for (i = 0; i < list.size(); i++) 
  delete list[i];

  list.empty();
}

void MenuContainer::AddButton(string Text, cCallback *recallFunc, bool enabled, int ID) {
  Button *tempButton;

  tempButton = new Button(Text, recallFunc, enabled, ID);
  list.push_back((DataContainer*)tempButton);

  BottomRightY += 18;
  if ((BottomRightX - TopLeftX) < (Text.length() * 16))
  BottomRightX = TopLeftX + (Text.length() * 16);
}

void MenuContainer::AddButton(string Text, cCallback *recallFunc, bool enabled, int ID, float red, float green, float blue) {
  Button *tempButton;

  tempButton = new Button(Text, red, green, blue, recallFunc, enabled, ID);
  list.push_back((DataContainer*)tempButton);

  BottomRightY += 18;
  if((BottomRightX - TopLeftX) < (Text.length() * 16))
    BottomRightX = TopLeftX + (Text.length() * 16);
}

void MenuContainer::AddData(string Text) {
  DataContainer *tempContainer;

  tempContainer = new DataContainer(Text);
  list.push_back(tempContainer);

  BottomRightY += 18;
  if ((BottomRightX - TopLeftX) < (Text.length() * 16))
  BottomRightX = TopLeftX + (Text.length() * 16);
}

void MenuContainer::AddData(string Text, float red, float green, float blue) {
  DataContainer *tempContainer;

  tempContainer = new DataContainer(Text, red, green, blue);
  list.push_back(tempContainer);

  BottomRightY += 18;
  if((BottomRightX - TopLeftX) < (Text.length() * 16))
    BottomRightX = TopLeftX + (Text.length() * 16);
}

void MenuContainer::Display() {
  unsigned int i;
  float alignTranslate;

  glPushMatrix();
  
  glLoadIdentity();

  glTranslatef(TopLeftX, TopLeftY, 0.0);

  for (i = 0; i < list.size(); i++) {
    alignTranslate = 0.0;
    if(align == maCenter) {
      // If the text is centered, we need to move it left half its length
      alignTranslate = (float)((list[i]->GetLength() * 10) / 2);
    } else if(align == maRight) {
      // If the text is right alligned, move it left the whole length
      alignTranslate = (float)(list[i]->GetLength() * 10);
    }
    glPushMatrix();
    glTranslatef((-1.0) * alignTranslate, 0, 0);
    list[i]->Display();
    glPopMatrix();
    glNewLine();
  }
  glPopMatrix();
}

bool MenuContainer::LocationWithin(int x, int y) {

  if ((x >= TopLeftX) && (x <= BottomRightX) && (y >= TopLeftY) && (y <= BottomRightY)) return true;

  return false;

  /*
  for (i = 0; i < list.size(); i++) {
    if (list[i]->Selectable()) {
      tempItem = (Button*)list[i];
      if ((locY > (TopLeftY + (18 * i)) && (locY < (TopLeftY + (18 * (i + 1)))))) {

      tempItem->Select();

      return true;
    } 
  }
}
  return false;
*/
}

void MenuContainer::SelectionDown() {
  unsigned int i;
  Button *tempButton;

  if (!selected) return;

  for (i = button + 1; i < list.size(); i++) {
    if (list[i]->Selectable()) {
      tempButton = (Button*)list[i];
      tempButton->Select();
      tempButton = (Button*)list[button];
      tempButton->Deselect();
      button = i;
      return;
    }
  }

  for (i = 0; (int) i < button; i++) {
    if (list[i]->Selectable()) {
      tempButton = (Button*)list[i];
      tempButton->Select();
      tempButton = (Button*)list[button];
      tempButton->Deselect();
      button = i;
      return;
    }
  }
}

void MenuContainer::SelectionUp() {
  int i;
  Button *tempButton;

  if (!selected) return;

  for (i = button - 1; i >= 0; i--) {
    if (list[i]->Selectable()) {
      tempButton = (Button*)list[i];
      tempButton->Select();
      tempButton = (Button*)list[button];
      tempButton->Deselect();
      button = i;
      return;
    }
  }

  for (i = (int)list.size() - 1; i > button; i--) {
    if (list[i]->Selectable()) {
      tempButton = (Button*)list[i];
      tempButton->Select();
      tempButton = (Button*)list[button];
      tempButton->Deselect();
      button = i;
      return;
    }
  }
}

bool MenuContainer::Select() {
  unsigned int i;
  Button* tempButton;

  if (selected) return true;

  for (i = 0; i < list.size(); i++) {
    if (list[i]->Selectable()) {
      button = i;
      tempButton = (Button*)list[i];
      tempButton->Select();
      selected = true;
      return true;
    }
  }

  return false;
}

void MenuContainer::Deselect() {
  Button *tempButton;

  if (!selected) return;

  tempButton = (Button*)list[button];
  tempButton->Deselect();
  button = -1;
  selected = false;
}

string MenuContainer::GetName() {
  return myName;
}

void MenuContainer::Execute() {
  Button *tempButton;

  if (!selected) {
    Log::logger->message("Error: MenuContainer not Selected!");
    return;
  }

  tempButton = (Button*)list[button];
  tempButton->Execute();
}

void MenuContainer::Print() {
  unsigned int i;

  Log::logger->message("-----------------");
  Log::logger->message("List:\t%s", myName.c_str());
  Log::logger->message("-----------------");
  for (i = 0; i < list.size(); i++)
  list[i]->Print();
}

void MenuContainer::ChangeEnabled(string Text, bool enabled) {
  unsigned int i;
  Button* tempButton;

  for (i = 0; i < list.size(); i++) {
    if (list[i]->GetText() == Text) {
      tempButton = (Button*) list[i];
      tempButton->ChangeEnabled(enabled);
    }
  }
}

bool MenuContainer::MouseSelect(int x, int y) {
  unsigned int i;
  Button* tempItem;

  for (i = 0; i < list.size(); i++) {
    if (list[i]->Selectable()) {
      tempItem = (Button*)list[i];
        if ((y > (TopLeftY + (18 * i)) && (y < (TopLeftY + (18 * (i + 1)))))) {

        // This short circuits the problem of
        // hovering. If you hover over a button
        // without this line, it will always
        // select and deselect the same button...
        // not a good thing.
        if (button == (int) i) return true;
      
        tempItem->Select();
        if (button != -1) {
        tempItem = (Button*)list[button];
        tempItem->Deselect();
      }
      button = i;

      return true;
    }
  }
}
  return false;
}
