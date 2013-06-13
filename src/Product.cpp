/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <iostream>

#include "Economy.h"
#include "Product.h"

using namespace std;

Product::Product() {
  ID = -1;
  medianPrice = 0.0;
  demand = 0;
}

Product::Product(int myID, float myMedianPrice, int myDemand) {
  ID = myID;
  medianPrice = myMedianPrice;
  demand = myDemand;
}

Product::~Product() {
}

void Product::SetMedianPrice(float Price) {
  medianPrice = Price;
}

void Product::SetDemand(int Demand) {
  demand = Demand;
}

int Product::IncreaseDemand() {
  ++demand;
  return demand;
}

int Product::IncreaseDemand(int amount) {
  demand += amount;
  return demand;
}

int Product::DecreaseDemand() {
  --demand;
  return demand;
}

int Product::DecreaseDemand(int amount) {
  demand -= amount;
  return amount;
}

string Product::GetName() {
  return Economy::Econ->ProdList[ID];
}

float Product::GetMedianPrice() {
  return medianPrice;
}

int Product::GetDemand() {
  return demand;
}

void Product::Print() {
  cout << "Product Name: " << GetName() << "\n\tPrice: " << medianPrice << "\n\tDemand: " << demand
       << endl;
}
