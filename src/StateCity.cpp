/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "Config.h"
#include "Log.h"
#include "image.h"
#include "font.h"
#include "GameState.h"
#include "StateSailing.h"
#include "City.h"
#include "Menu.h"
#include "Economy.h"
#include "Product.h"
#include "StateCity.h"

using namespace std;

StateCity::StateCity(City *townIn, Player *data) : GameState(NULL) {
  stringstream tempStr;
  unsigned int i;
  int ID;

  exit_callback.SetCallback(this, &StateCity::exit);
  buy_callback.SetCallback(this, &StateCity::buy);
  sell_callback.SetCallback(this, &StateCity::sell);

  // save the location where the player is.
  town = townIn;
  
  // Save the player information
  PlayerInfo = data;

  menu = new Menu("data/city.png", mainMenu, this);

  menu->addList("CityName", 0.5, 0.2);
  menu->addList("Menu", 0, .4);
  menu->addList("Product", .5, .4, maRight);
  menu->addDataItem("Products", "Product");
  menu->addDataItem("--------", "Product");
  menu->addList("Price", .6, .4);
  menu->addDataItem("Prices", "Price");
  menu->addDataItem("--------", "Price");
  menu->addList("Buy", .7, .4);
  menu->addDataItem("", "Buy");
  menu->addDataItem("", "Buy");
  menu->addList("Sell", .8, .4);
  menu->addDataItem("", "Sell");
  menu->addDataItem("", "Sell");

  //menu->addDataItem(town->getName(), "CityName");
  menu->addDataItem(town->getName(), "CityName", 1.0, 0.0, 0.0);

  ID = menu->addMenuItem("Exit", &exit_callback, "Menu", true);
  if(ID == (int)ButtonList.size()) ButtonList.push_back(town->getName()); 

  productList = town->ProductInformation();

  /* To display the information, I decided to use vectors
   * and stringstreams to get the information into the lists
   * The stringstream is used to convert the numbers into
   * strings. This seems to work jusssst nice.
   */
  for(i = 0; i < productList.size(); i++) {
    menu->addDataItem(productList[i]->GetName(), "Product");
    tempStr << productList[i]->GetMedianPrice();
    menu->addDataItem(tempStr.str(), "Price");

    ID = menu->addMenuItem("Buy", &buy_callback, "Buy", true);
    if (ID == (int)ButtonList.size()) ButtonList.push_back(productList[i]->GetName());

    ID = menu->addMenuItem("Sell", &sell_callback, "Sell", true);
    if (ID == (int)ButtonList.size()) ButtonList.push_back(productList[i]->GetName());

    tempStr.str(""); // We have to empty the sstream

    tempStr << productList[i]->GetDemand();
    menu->addDataItem(tempStr.str(), "Demand");

    tempStr.str(""); // We have to empty the sstream
  }
}

StateCity::~StateCity() {
  delete menu;
}

void StateCity::loop() {
  menu->loop();
}

void StateCity::exit(int ID) {
  switchState(previous, this);
}

void StateCity::buy(int ID) {
  int prodID;
  unsigned int i;

  for(i = 0; i < Economy::Econ->ProdList.size(); i++) {
    if(ButtonList[ID] == Economy::Econ->ProdList[i]) {
      prodID = i;
      break;
    }
  }

  Economy::Econ->BuyProduct(PlayerInfo, prodID, productList[prodID]->GetMedianPrice());
}

void StateCity::sell(int ID) {
  int prodID;
  unsigned int i;

  for(i = 0; i < Economy::Econ->ProdList.size(); i++) {
    if(ButtonList[ID] == Economy::Econ->ProdList[i]) {
      prodID = i;
      break;
    }
  }

  Economy::Econ->SellProduct(PlayerInfo, prodID, productList[prodID]->GetMedianPrice());
}

void StateCity::switchTo(GameState *oldState) {
  menu->switchTo(oldState);
}

void StateCity::switchFrom() {
}
