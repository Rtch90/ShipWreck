/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <string>
#include <exception>
#include "image.h"
#include "Log.h"
#include "tinyxml.h"
#include "Model.h"
#include "Cargo.h"
#include "Ships.h"

using namespace std;

Ship::Ship(ShipInfo *infoIn, string shipTitle) {
  unsigned int cargoSize;

  info =  infoIn;

  info->addRef();

  x     = 0;
  z     = 0;
  rot   = 0;
  speed = 0;

  title = shipTitle;

  damage = info->getMaxDamage();

  cargoSize = info->getCargoSize();
  cargo     = new Cargo(title + " Cargo", cargoSize);
}

Ship::Ship(Ship *copy) {
  info = copy->info;
  info->addRef();

  x     = copy->x;
  z     = copy->z;
  rot   = copy->rot;
  speed = copy->speed;

  title  = copy->title;

  damage = copy->damage;

  cargo  = copy->cargo;
}

Ship::~Ship() {
  info->deleteRef();
  
  info = NULL;
}

void Ship::draw() {
  info->draw();
}

int Ship::getMaxDamage() {
  return info->getMaxDamage();
}

void Ship::dump() {
  cargo->dump();
}

// ShipInfo code

ShipInfo::ShipInfo(string nameIn, string modelFile, string texFile, float scaleIn, int damageIn, unsigned int cargoSizeIn) {
  
  modelFileName   = modelFile;
  textureFileName = texFile;

  refCount = 0;

  model = NULL;

  // Set up ship stats
  name      = nameIn;
  scale     = scaleIn;
  maxDamage = damageIn;
  cargoSize = cargoSizeIn;
}

ShipInfo::~ShipInfo() {
  if(refCount != 0) {
    unload();
    Log::logger->message("Warning: ShipInfo destructor called with refCount != 0");
  }
}

void ShipInfo::load() {
  if(model != NULL) {
    Log::logger->message("Warning: Loading already loaded ship");
  }

  model = new Model(textureFileName.c_str());
  model->loadObj(modelFileName.c_str(), scale);
}

void ShipInfo::unload() {
  if(model == NULL) {
    Log::logger->message("Warning: Unloading already unloaded ship");
    return;
  }

  delete model;
  model = NULL;
}

void ShipInfo::addRef() {
  // If refCount is z before increment, load the model
  if(refCount++ == 0) {
    load();
  }
}


void ShipInfo::deleteRef() {
  if(--refCount <= 0) {
    unload();
    refCount = 0;
  }
}

void ShipInfo::draw() {
  model->draw();
}

int ShipInfo::getMaxDamage() {
  return maxDamage;
}

unsigned int ShipInfo::getCargoSize() {
  return cargoSize;
}

// ShipManager Code
ShipManager* ShipManager::list = NULL;

ShipManager::ShipManager(string file) {
  LoadXMLFile(file);
}

ShipManager::~ShipManager() {
  unsigned int i;

  for(i=0; i<ships.size(); i++) {
    delete ships[i];

    ships[i] = NULL;
  }
}

Ship* ShipManager::newShip(unsigned int index, string shipTitle) {
  if(index >= ships.size()) {
    // Should throw an exception
    Log::logger->message("Warning: getShip index out of bounds");
    return NULL;
  }

  return new Ship(ships[index], shipTitle);
}

void ShipManager::LoadXMLFile(string file) {
  TiXmlDocument doc( file.c_str() );
  TiXmlNode *node;
  TiXmlElement *temp;
  TiXmlText *text;
  string name, modelName, texName;
  float scale;
  int damage;
  unsigned int cargoSize;
  ShipInfo *tempShip;

  doc.LoadFile();
  if(doc.Error()) {
    // Should throw an exception
    Log::logger->message("Warning: %s (line %d)", doc.ErrorDesc(), doc.ErrorRow());
    return;
  }


  for(node = doc.FirstChildElement("Ship"); node; node = node->NextSibling("Ship")) {
    try {
      temp = node->FirstChildElement("Name");
      if(temp == NULL) throw "Name";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Name";
      name = text->Value();

      temp = node->FirstChildElement("Model");
      if(temp == NULL) throw "Model";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Model";
      modelName = text->Value();

      temp = node->FirstChildElement("Texture");
      if(temp == NULL) throw "Texture";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Texture";
      texName = text->Value();

      temp = node->FirstChildElement("Scale");
      if(temp == NULL) throw "Scale";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Scale";
      if(sscanf(text->Value(), "%f", &scale) != 1) throw "Scale";

      temp = node->FirstChildElement("Damage");
      if(temp == NULL) throw "Damage";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Damage";
      if(sscanf(text->Value(), "%d", &damage) != 1) throw "Damage";

      temp = node->FirstChildElement("CargoSize");
      if(temp == NULL) throw "Cargo Size";
      text = temp->FirstChild()->ToText();
      if(text == NULL) throw "Cargo Size";
      if(sscanf(text->Value(), "%u", &cargoSize) != 1) throw "Cargo Size";

      // Ready to create the ship and add it to the ship list
      tempShip = new ShipInfo(name, modelName, texName, scale, damage, cargoSize);
      ships.push_back(tempShip);

    } catch (const char *e) {
      Log::logger->message("Warning: Problem reading %s ship property in %s", e, file.c_str());
    }
  }
}

void ShipManager::initShipList(string file) {
  // Make sure shipList hasn't already been initialized
  if(list != NULL) {
    Log::logger->message("Warning: Multiple calls to initShipList");
    return;
  }
  
  list = new ShipManager(file);
}

void ShipManager::deleteShipList() {
  if(list == NULL) {
    Log::logger->message("Warning: Call to deleteShipList with NULL shipList pointer");
    return;
  }
  delete list;
  list = NULL;
}
