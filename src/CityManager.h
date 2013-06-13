/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _CITYMANAGER_H_
#define _CITYMANAGER_H_

#include <vector>

// Just so I do not have to include any defines
class EnvironmentObject;
class Player;
class City;
class Terrain;

using namespace std;

class CityManager {
public:
  // Constructor!
  CityManager(Terrain *land);
  
  // Destructor
  ~CityManager();

  // Methods
  City* CheckCollision(Player *player);

  void DisplayCities();
  void MapDisplay();

  bool CheckCity(const char *cityName);

private:
  vector<City*> CityList;
  void LoadCityConfig(const char *filename, Terrain *land);
};

#endif
