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
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include "ccmath.h"
#include "City.h"
#include "EnvironmentObject.h"
#include "Terrain.h"
#include "Player.h"
#include "Log.h"
#include "Product.h"
#include "CityManager.h"
#include "tinyxml.h"

#define MAX_CITIES 32 // This MAY need to be bigger soon.

using namespace std;

CityManager::CityManager(Terrain *land) {
  LoadCityConfig("data/City.xml", land);

  //WorldEcon = new Economy("data/Product.xml");
}

CityManager::~CityManager() {
  CityList.clear();
}

City* CityManager::CheckCollision(Player *player) {
  // Used to determine if the player is sailing towards the city
  float angleToCity;
  float diffAngles;

  unsigned int i;

  // No sense in checking if the player isn't moving
  if(player->speed == 0) return NULL;

  for(i=0; i<CityList.size(); i++) {
    // Check the distance to each city
    // This isn't really efficient, but since there aren't many cities 
    // it's not really worth doing something fancier.
    if(CityList[i]->calcDist(player->x, player->z) < 3) {
      // It's in range, now make sure the angle is close

      angleToCity = CityList[i]->calcAngle(player->x, player->z);

      diffAngles = abs(player->rot - angleToCity);
      if(diffAngles > M_PI) diffAngles = 2*M_PI - diffAngles;

      if(diffAngles < degreesToRadians(15)) {
        // The player is close and headed +-5 degrees of city
        return CityList[i];
      }
    }
  }

  // No collision
  return NULL;
}

void CityManager::DisplayCities() {
  unsigned int i;
  
  // Save some gl calls, just do this once for all the cities
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_COLOR_MATERIAL);
  glColor4f(1, 1, 0, .4);
  
  // I'm not sure why, but if you don't do this, then City1 
  // (CityList[0]) doesn't get displayed.  It's probably not worth worrying 
  // about since we will hopefully be getting city models soon.
  glBegin(GL_QUADS);
  glEnd();

  for(i = 0; i < CityList.size(); i++) {
    CityList[i]->DisplayObject();
  }

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
}

void CityManager::MapDisplay() {
  unsigned int i;
  
  for(i = 0; i < CityList.size(); i++) {
    CityList[i]->MapDisplay();
  }
}

void CityManager::LoadCityConfig(const char *filename, Terrain *land) {
  City* TempCity;
  TiXmlElement* object;
  TiXmlElement* location;
  TiXmlNode* node;
  TiXmlDocument doc( filename );
  TiXmlText* cityString;
  string cityName;
  vector<Product*> tempList;
  int locX, locZ;
  int objNum = 0;

  doc.LoadFile();
  node = doc.FirstChild();

  for(node = node->NextSibling(); node; node = node->NextSibling()) {
    objNum++;
    try {
      // Get the name of the city
      object = node->FirstChild("name")->ToElement();
      if(object == NULL) throw 0;
      cityString = object->FirstChild()->ToText();
      if(cityString == NULL) throw 0;
      //cityString = object->ToText();
      cityName = cityString->Value();

      // Get the X location
      location = node->FirstChild("location")->ToElement();
      if(location == NULL) throw 1;
      object = location->FirstChild("x")->ToElement();
      if(object == NULL) throw 1;
      cityString = object->FirstChild()->ToText();
      if(cityString == NULL) throw 1;
      sscanf(cityString->Value(), "%d", &locX);

      // Get the Z location
      object = location->FirstChild("z")->ToElement();
      if(object == NULL) throw 2;
      cityString = object->FirstChild()->ToText();
      if(cityString == NULL) throw 2;
      sscanf(cityString->Value(), "%d", &locZ);

      TempCity = new City(land, cityName, locX, locZ);

      CityList.push_back(TempCity);
    } catch(int j) {
      switch (j) {
      case 0:
        Log::logger->message("Warning: Problem with Object Name.");
        break;
      case 1:
        Log::logger->message("Warning: Problem with Object x Location.");
        break;
      case 2:
        Log::logger->message("Warning: Problem with Object y Location.");
        break;
      default:
        Log::logger->message("Warning: Problem with City Config file.");
        break;
      }
    }

  }
}

bool CityManager::CheckCity(const char *cityName) {
  unsigned int i;

  for(i = 0; i < CityList.size(); i++) {
    if(CityList[i]->CheckName(cityName)) return true;
  }    

  return false;
}
