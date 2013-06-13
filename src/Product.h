/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _PRODUCT_H_
#define _PRODUCT_H_

#include <string>

using namespace std;

class Product {
public:
  // Constructor
  Product();
  Product(int myID, float myMedianPrice, int myDemand);

  // Destructor
  ~Product();

  //methods
  void SetMedianPrice(float Price);
  void SetDemand(int Demand);
  
  // Without an argument, the increase is one
  int IncreaseDemand();
  int IncreaseDemand(int amount);

  // Without an argument, the decrease is one
  int DecreaseDemand();
  int DecreaseDemand(int amount);
  
  // these get the private data
  string GetName();
  float GetMedianPrice();
  int GetDemand();

  void Print();

private:
  int ID;
  float medianPrice;
  int demand;
};

#endif
