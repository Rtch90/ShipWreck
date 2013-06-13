/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _MENUCONTAINER_H_
#define _MENUCONTAINER_H_

#include <string>
#include <vector>
#include "../Callback.h"
#include "DataContainer.h"
#include "Button.h"

using namespace std;

class MenuContainer {
public:
  // Constructor
  MenuContainer(string name, float x, float y, int alignment);

  // Destructor
  ~MenuContainer();

  // Methods
  void AddButton(string Text, cCallback *recallFunc, bool enabled, int ID);
  void AddButton(string Text, cCallback *recallFunc, bool enabled, int ID, float red, float green, float blue);
  void AddData(string Text);
  void AddData(string Text, float red, float green, float blue);

  // These two methods are for mouse handling.
  // The first simply returns if the mouse is
  // within the MenuContainer. The second should
  // only get called if the first returns true.
  // MouseSelect changes the selection if 
  // appropriate and returns true, or it does 
  // nothing and returns false.
  bool LocationWithin(int x, int y);
  bool MouseSelect(int x, int y);

  void Display();

  // These methods move the selected item up and down the list
  void SelectionDown();
  void SelectionUp();

  // This tells this list that it is selected and can display 
  // one of its items as such. It returns true if it has some
  // selectable items and false if not.
  bool Select();
  void Deselect();

  void ChangeEnabled(string Text, bool enabled);

  string GetName();

  void Execute();

  void Print();
private:
  vector<DataContainer*> list;
  string myName;
  float locX;
  float locY;

  int align;

  float TopLeftX;
  float TopLeftY;
  float BottomRightX;
  float BottomRightY;

  int button;

  bool selected;

};

#endif
