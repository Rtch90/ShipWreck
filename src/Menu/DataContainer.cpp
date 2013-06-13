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

using namespace std;

DataContainer::DataContainer(string Text) {
  myText     = Text;
  redColor   = 0.2;
  blueColor  = 0.2;
  greenColor = 0.2;

  amISelectable = false;
}

DataContainer::DataContainer(string Text, float red, float green, float blue) {
  myText     = Text;
  redColor   = red;
  blueColor  = blue;
  greenColor = green;
  
  amISelectable = false;
}

DataContainer::~DataContainer() {
}

void DataContainer::ChangeText(string Text) {
  myText = Text;
}

string DataContainer::GetText() {
  return myText;
}

void DataContainer::Display() {
  float color[4];

  glGetFloatv(GL_CURRENT_COLOR, color);

  glColor3f(redColor, greenColor, blueColor);
  glPrint(myText.c_str(), 0);

  glColor3f(color[0], color[1], color[2]);
}

bool DataContainer::Selectable() {
  return amISelectable;
}

void DataContainer::Print() {
  Log::logger->message("\n\tText:\t%s", myText.c_str());
  if(amISelectable)
    Log::logger->message("\tSelectable:\tTrue");
  else
    Log::logger->message("\tSelectable:\tFalse");
  Log::logger->message("\tColor RGB:\t%i %i %i", redColor, greenColor, blueColor);
}

int DataContainer::GetLength() {
  return (int)myText.length();
}
