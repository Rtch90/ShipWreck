/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

// This is inherited from the environment object. It will
// keep the lacation, name, and economic information of the city.

#ifndef _CITY_H_
#define _CITY_H_

#include <vector>
#include "EnvironmentObject.h"

class Product;
class Terrain;
class CityEconomy;

using namespace std;

class City: public EnvironmentObject {
public:
  // Constructor
  City(Terrain *land, string cityName, int cityLocX, int cityLocY);
  
  //Destructor
  ~City();

  // Method
  void DisplayObject();
  void MapDisplay();

  vector<Product*> ProductInformation();

  bool CheckName(const char* cityName);

private:
  // We can put product information and crap here, whatever.
  float height;
  CityEconomy* MyEconomy;
};

#endif
