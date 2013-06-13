/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../font.h"
#include "../Log.h"
#include "DataContainer.h"
#include "Button.h"

using namespace std;

Button::Button(string Text, cCallback *recallFunc, bool enabled,
	       int ID): DataContainer(Text) {
  reFunc     = recallFunc;
  amIEnabled = enabled;
  myID       = ID;
  selected   = false;

  if(enabled) {
    redColor    = .8;
    blueColor   = .8;
    greenColor  = .8;
  }

  permRed       = redColor;
  permBlue      = blueColor;
  permGreen     = greenColor;
  amISelectable = true;
}

Button::Button(string Text, float red, float green, float blue,
	       cCallback *recallFunc, bool enabled,
	       int ID): DataContainer(Text, red, green, blue) {
  reFunc        = recallFunc;
  amIEnabled    = enabled;
  myID          = ID;
  selected      = false;

  permRed       = red;
  permGreen     = green;
  permBlue      = blue;
  amISelectable = true; 
}

Button::~Button() {
}

void Button::ChangeCallback(cCallback *recallFunc) {
  reFunc = recallFunc;
}

void Button::ChangeEnabled(bool enabled) {
  amIEnabled = enabled;

  if(enabled) {
    redColor   = .8;
    blueColor  = .8;
    greenColor = .8;
  } else {
    redColor   = .2;
    blueColor  = .2;
    greenColor = .2;
  }

  permRed   = redColor;
  permBlue  = blueColor;
  permGreen = greenColor;
}

void Button::Execute() {
  reFunc->Execute(myID);
}

bool Button::GetEnabled() {
  return amIEnabled;
}

int Button::GetID() {
  return myID;
}

void Button::Select() {
  selected = true;
  
  redColor   = 0;
  greenColor = 1;
  blueColor  = 1;
}

void Button::Deselect() {
  selected = false;

  redColor   = permRed;
  greenColor = permGreen;
  blueColor  = permBlue;
}

bool Button::Selectable() {
  return amIEnabled;
}

void Button::Print() {
  Log::logger->message("\n\tText:\t%s", myText.c_str());
  if (amIEnabled)
    Log::logger->message("\tSelectable:\tTrue");
  else 
    Log::logger->message("\tSelectable:\tFalse");
  if (selected)
    Log::logger->message("\tSelected:\tTrue");
  else 
    Log::logger->message("\tSelected:\tFalse");
  Log::logger->message("\tID:\t%i", myID);
  Log::logger->message("\tColor RGB:\t%i %i %i", redColor, greenColor, blueColor);
}
