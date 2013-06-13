/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 * --------------------------------------------------------------------------- 
 * The Economy class is the top level of all the
 * economies present in Lephisto. It is the authority
 * on how products and prices change.
 *
 * After the creation of the Economy, the economy
 * doesn't interact much with other classes. Each
 * City will have a pointer to an element within
 * the economy so the city can access and change
 * the information if it wants to. The only thing that
 * needs to be called is Step() once and a while
 * so that the Economy will actualy change.
 */

#ifndef _ECONOMY_H_
#define _ECONOMY_H_

#include <vector>
#include <string>

#include "tinyxml.h"

//class Product;
class CityManager;
class CityEconomy;
class Player;

using namespace std;

class Economy {
public:
  /* Constructor
   * the constructor creates the Economy and opens
   * the file in filename. It does not create the
   * CityEconomies, they are created using CreateCity.
   * This allows a basic error checking system so that
   * there aren't cities within Economies and no
   * Economies without cities.
   */

  Economy(const char* filename);
  
  // destructor
  ~Economy();

  // Methods
  bool CreateEcon();  // This creates the Economy for the static Economy
  // This changes the state of the economy
  void Step();
  void Print();
 
  CityEconomy* CreateCity(const char* CityName);

  static Economy* Econ;

  bool CheckCities(CityManager* tempManager);
  
  int ProductCount();

  void BuyProduct(Player *data, int ProdID, float price);
  void SellProduct(Player *data, int ProdID, float price);

  vector<string> ProdList;

private:
  int FindID(string Name);
  vector<CityEconomy*> CityList;

  TiXmlDocument doc;
  TiXmlNode* EconomyNode;
};

#endif