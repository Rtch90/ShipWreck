/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <string>

#include "../Callback.h"
#include "DataContainer.h"

using namespace std;

class Button: DataContainer {
public:
  // Constructor
  Button(string Text, cCallback *recallFunc, bool enabled, int ID);
  Button(string Text, float red, float green, float blue,
	 cCallback *recallFunc, bool enabled, int ID);

  // Deconstructor
  ~Button();

  // Methods
  void ChangeEnabled(bool enabled);
  bool GetEnabled();
  
  void ChangeCallback(cCallback *recallFunc);

  int GetID();

  void Select();
  void Deselect();

  void Execute();

  bool Selectable();
  
  void Print();

private:
  cCallback *reFunc;

  bool amIEnabled;
  int myID;
  
  bool selected;

  float permRed;
  float permGreen;
  float permBlue;
};

#endif
