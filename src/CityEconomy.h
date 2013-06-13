/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 *
 * This is a container for each of the citys to keep teack of the state
 * of its own economy. It also allows the economy class to work on it.
 * Each instance is kept in the Economy class and the cities must access it
 * through the economy interface.
 */

#ifndef _CITYECONOMY_H_
#define _CITYECONOMY_H_

#include <vector>
#include <string>

class Product;

using namespace std;

class CityEconomy {
public:
  // Constructor
  CityEconomy(const char* CityName, vector<Product*> ProductList);

  // Destructor
  ~CityEconomy();

  // Methods
  string getCityName();

  Product* getProduct(char*ProductName);
  float getPrice(char* ProductName);

  void Print();

  vector<Product*> getProductList();

  // Bunch of other crap?
private:
  vector<Product*> myProducts;
  string myName;
};

#endif
