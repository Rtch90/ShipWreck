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
#include "Player.h"
#include "Cargo.h"
#include "Product.h"
#include "Log.h"
#include "tinyxml.h"
#include "CityManager.h"
#include "CityEconomy.h"
#include "Economy.h"

using namespace std;

Economy *Economy::Econ = NULL;

Economy::Economy(const char* filename) {
  TiXmlNode*    Products;
  TiXmlNode*    ForProducts;
  TiXmlElement* Prod;
  TiXmlText*    data;
  string        prodName;

  doc.LoadFile(filename);
  if(doc.Error()) {
    Log::logger->message("Error: Could not open Economy file.\n");
    exit(-1);
  }

  Products = doc.FirstChild("Products");

  for(ForProducts = Products->FirstChildElement("Name");
      ForProducts;
      ForProducts = ForProducts->NextSibling("Name")) {

    if(ForProducts == NULL) throw -1;
    Prod = ForProducts->ToElement();
    data = Prod->FirstChild()->ToText();
    prodName = data->Value();

    ProdList.push_back(prodName);
  }
}

Economy::~Economy() {
  unsigned int i;
  CityEconomy* temp;
  
  for(i = 0; i < CityList.size(); i++) {
    temp = (CityEconomy*)CityList[i];
    delete temp;
  }

  CityList.clear();
}

bool Economy::CreateEcon() {
  if(Econ != NULL) {
    Log::logger->message("Warning: Multiple calls to CreateEcon()");
    return false;
  }

  Econ = new Economy("data/Product.xml");

  return true;
}

void Economy::Step() {
}

void Economy::Print() {
  unsigned int i;
  CityEconomy* temp;

  for(i = 0; i < CityList.size(); i++) {
    temp = (CityEconomy*) CityList[i];
    temp->Print();
  }
}

CityEconomy* Economy::CreateCity(const char* CityName) {
  TiXmlNode*        City;
  TiXmlElement*     CityElement;
  TiXmlElement*     Prod;
  TiXmlElement*     Value;
  TiXmlElement*     Demand;
  TiXmlText*	    data;
  TiXmlNode*        node;
  Product*          tempProduct;
  vector<Product*>  ProductList;
  CityEconomy*	    tempCE;
  string            name;
  string            cityName;
  float             price;
  int               demand;
  int               proNum;
  int               n;
  int               ID;

  EconomyNode = doc.FirstChild("Economy");

  for(City = EconomyNode->FirstChildElement("City");
      City;
      City = City->NextSibling("City")) {
    
    if(City == NULL) throw -1;
    CityElement = City->ToElement();
    cityName = CityElement->Attribute("name");
    if(cityName == CityName) {

      for(node = City->FirstChild("Product"); node; node = node->NextSibling("Product")) {
        if(node == NULL) throw 0;
        proNum++;
        try {
          Prod = node->FirstChildElement("Name");
          if(Prod == NULL) throw 1;
          data = Prod->FirstChild()->ToText();
          if(data == NULL) throw 1;
          name = data->Value();
          ID = FindID(name);

          Value = node->FirstChildElement("Value");
          if(Value == NULL) throw 2;
          data = Value->FirstChild()->ToText();
          if(data == NULL) throw 2;
          n = sscanf(data->Value(), "%f", &price);
          if(n == -1) {
            Log::logger->message("Error: Bad XML document.\n");
            throw 2;
          }

          Demand = node->FirstChildElement("Demand");
          if(Demand == NULL) throw 3;
          data = Demand->FirstChild()->ToText();
          if(data == NULL) throw 3;
          n = sscanf(data->Value(), "%i", &demand);
          if(n == -1) throw 3;

          tempProduct = new Product(ID, price, demand);

          ProductList.push_back(tempProduct);
        } catch(int i) {

          switch (i) {
          case 0:
            Log::logger->message("Warning: <Product> did not parse correctly.");
            break;
          case 1:
            Log::logger->message("Warning: <Name> did not parse correctly.");
            City->Print(stdout, 0);
            break;
          case 2:
            Log::logger->message("Warning: <Value> did not parse correctly.");
            City->Print(stdout, 0);
            break;
          case 3:
            Log::logger->message("Warning: <Demand> did not parse correctly.");
            City->Print(stdout, 0);
            break;
          default:
            Log::logger->message("Warning: Unknown error occured.\n");
            break;
          }
        } // Catch (int i)
      } // for loop
      tempCE = new CityEconomy(cityName.c_str(), ProductList);
      ProductList.clear();
      CityList.push_back(tempCE);
      break;
    } // if(cityName == CityName)
  } // For loop
  return tempCE;
}

bool Economy::CheckCities(CityManager* tempManager) {
  unsigned int i;
  for(i = 0; i < CityList.size(); i++) {
    if(!tempManager->CheckCity(CityList[i]->getCityName().c_str()))
    return false;
  }

  return true;
}

int Economy::ProductCount() {
  return (int)ProdList.size(); 
}

void Economy::BuyProduct(Player *data, int ProdID, float price) {
  Cargo *playerCargo = data->getCargo();

  try {
    // Make sure the player can afford it, and has room for it
    if((data->gold < price) || (playerCargo->getAvailQty() <= 0)) return;

    playerCargo->addQty(ProdID, 1);
    data->gold -= (int) price;
    Log::logger->message("Purchased %s", ProdList[ProdID].c_str());
  } catch (const char *e) {
    Log::logger->message(e);
  }
}

void Economy::SellProduct(Player *data, int ProdID, float price) {
  Cargo *playerCargo = data->getCargo();

  try {
    if(playerCargo->getQty(ProdID) <= 0) return;

    playerCargo->addQty(ProdID, -1);
    data->gold += (int) price;
    Log::logger->message("Sold %u", ProdList[ProdID].c_str());
  } catch (const char *e) {
    Log::logger->message(e);
  }
}

int Economy::FindID(string Name) {
  unsigned int i;

  for(i = 0; i < ProdList.size(); i++) {
    if(ProdList[i] == Name) return (int) i;
  }

  return -1;
}
