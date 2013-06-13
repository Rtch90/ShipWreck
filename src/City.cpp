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
#include "EnvironmentObject.h"
#include "image.h"
#include "font.h"
#include "Terrain.h"
#include "Economy.h"
#include "Product.h"
#include "CityEconomy.h"
#include "Config.h"
#include "City.h"

using namespace std;

City::City(Terrain *land, string cityName, int cityLocX, int cityLocZ) : EnvironmentObject(cityName, cityLocX, cityLocZ, true) {
  int xDiff = (int) land->getWidth()  / 2;
  int zDiff = (int) land->getHeight() / 2;

  height = land->getHeight(cityLocX + xDiff, cityLocZ + zDiff);

  MyEconomy = Economy::Econ->CreateCity(cityName.c_str());
}

City::~City(){
}

void City::DisplayObject() {
  // Right now, we are just drawing a box where the city will be..

  float size = 1;

  //glDisable(GL_TEXTURE_2D);
  //glEnable(GL_BLEND);
  //glEnable(GL_COLOR_MATERIAL);
  //glColor4f(1, 1, 0, 4);

  glBegin(GL_QUADS);

  // Box top
  glNormal3f(0, 1, 0);
  glVertex3f(locX - size, height+5, locZ - size);
  glVertex3f(locX - size, height+5, locZ + size);
  glVertex3f(locX + size, height+5, locZ + size);
  glVertex3f(locX + size, height+5, locZ - size);

  // North
  glVertex3f(locX - size, height+0, locZ - size);
  glVertex3f(locX - size, height+5, locZ - size);
  glVertex3f(locX + size, height+5, locZ - size);
  glVertex3f(locX + size, height+0, locZ - size);

  // East
  glVertex3f(locX + size, height+0, locZ - size);
  glVertex3f(locX + size, height+5, locZ - size);
  glVertex3f(locX + size, height+5, locZ + size);
  glVertex3f(locX + size, height+0, locZ + size);

  // South
  glVertex3f(locX - size, height+0, locZ + size);
  glVertex3f(locX + size, height+0, locZ + size);
  glVertex3f(locX + size, height+5, locZ + size);
  glVertex3f(locX - size, height+5, locZ + size);

  // West
  glVertex3f(locX - size, height+0, locZ - size);
  glVertex3f(locX - size, height+0, locZ + size);
  glVertex3f(locX - size, height+5, locZ + size);
  glVertex3f(locX - size, height+5, locZ - size);

  // Do not need to draw the bottom.

  // This points from the origin to the city. helps to find the city.
  //glVertex3f(0,5,0);
  //glVertex3f(locX, 5, locZ);

  glEnd();
  //glEnable(GL_TEXTURE_2D);
  //glDisable(GL_BLEND);
  //glDisable(GL_COLOR_MATERIAL);
}

// woot, i have frets on fire again.

void City::MapDisplay() {
  float screenX, screenY;
  float multi;
  float winWidth, winHeight;
  const char* tempName; // = name.c_str();
  // Need an icon to display for each city with a name.. right now, it can be an X.

  // Our glprintf has some issues if you try to pass a std::string
  // so by changing the string to a character array, it eleminates this problem.

  tempName = name.c_str();

  /*
   * the difference in the grid from the map to the sailing screen
   * needs to be dealt with. You see a very similar piece of code
   * in Map.cpp to deal with where to draw the ship.
   */
  winWidth  = Config::configuration->getWinWidth();
  winHeight = Config::configuration->getWinHeight();

  multi   = winWidth / 812.0;
  screenX = (winWidth / 2) - ((-1 * locX) * multi);
  screenX -= 6; // the text does not put it EXACTLY where we want it, so we have to move it slightly.

  multi = winHeight / 500.0f;
  screenY = (winHeight / 2) - ((-1 * locZ) * multi);
  screenY -= 5; // Same again, we will need to mmove it slightly.

  glColor3f(1, 0, 0);

  glPushMatrix();
  glLoadIdentity();
  glTranslatef(screenX, screenY, 0);

  glPrintf(0, "x"); 
  glNewLine();
  glPrintf(0, "%s", tempName);
  glPopMatrix();
}

vector<Product*> City::ProductInformation() {
  return MyEconomy->getProductList();
}

bool City::CheckName(const char *cityName) {
  if(name.compare(cityName)) return true;

  return false;
}
