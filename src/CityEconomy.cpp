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
#include <iostream>
#include "Product.h"
#include "Log.h"
#include "CityEconomy.h"

using namespace std;

CityEconomy::CityEconomy(const char* CityName, vector<Product*> ProductList) {
  myProducts = ProductList;
  myName     = CityName;
}

CityEconomy::~CityEconomy() {
  unsigned int i;
  Product* tempProduct;
 
  for(i = 0; i < myProducts.size(); i++) {
    tempProduct = (Product*)myProducts[i];
    delete tempProduct;
  }
  
  myProducts.clear();
}

string CityEconomy::getCityName() {
  return myName;
}

Product* CityEconomy::getProduct(char* ProductName) {
  string compare;
  Product* tempProduct = NULL;
  unsigned int i;

  compare = ProductName;

  for(i = 0; i < myProducts.size(); i++) {
    tempProduct = (Product*)myProducts[i];
    if(compare == tempProduct->GetName()) break;
  }

  return tempProduct;

}

float CityEconomy::getPrice(char* ProductName) {
  string compare;
  Product* tempProduct = NULL;
  unsigned int i;

  compare = ProductName;

  for(i = 0; i < myProducts.size(); i++) {
    tempProduct = (Product*)myProducts[i];
    if(compare == tempProduct->GetName()) break;
  }

  if(tempProduct == NULL) {
    // This should probably throw an exception
    return 0;
  }

  return tempProduct->GetMedianPrice();
}

void CityEconomy::Print() {
  unsigned int i;
  Product* temp;

  cout << myName << ":\n";
  for(i = 0; i < myProducts.size(); i++) {
    temp = (Product*)myProducts[i];
    temp->Print();
  }
}

vector<Product*> CityEconomy::getProductList() {
  return myProducts;
}
