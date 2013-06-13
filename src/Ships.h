/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _SHIPS_H_
#define _SHIPS_H_

#include <string>
#include <vector>

using namespace std;

class Model;
class Cargo;

// This class holds information shared by all ships of certain types, such 
// as the model, total damage, etc..
class ShipInfo {
public:
  ShipInfo(string nameIn, string modelFile, string texFile, float scaleIn, int damageIn, unsigned int cargoSizeIn);

  ~ShipInfo();

  // Loading and unloading can be done automaticaly with addRef() and deleteRef()
  // Load the model and texture into memory
  void load();

  // Unload the model and texture
  void unload();

  // Add reference to ship, loads if necessary
  void addRef();

  // Removes reference, unloads if possible
  void deleteRef();

  void draw();

  int getMaxDamage();
  unsigned int getCargoSize();

 private:
  // Number of references
  int refCount;

  // Ship name
  string name;

  // File names
  string modelFileName;
  string textureFileName;

  // Data pointers
  Model *model;

  // Scale to draw ship
  float scale;

  // Max amount of damage before you are PWNED!!!!
  int maxDamage;

  // Size of the cargo hold
  unsigned int cargoSize;
};

// This is the public class that other programs use to handle a ship. 
// It takes care of adding and releasing references to the ShipInfo.
class Ship {
 public:
  Ship(ShipInfo *infoIn, string shipTitle);

  // Copy constructo
  // The ships will share the cargo object - Didn't your parents always teach you to share?
  Ship(Ship *copy);

  ~Ship();

  void draw();

  int getMaxDamage();

  // Dump some debug info please
  void dump();

  float x, z;
  float rot;
  float speed;
  int damage;
  string title;

  Cargo *cargo;

private:
  ShipInfo *info;
};

// This class maintains a list of all of the types of ships.
// When a new ship is needed, you should call newShip to get the ship type you want
class ShipManager {
public:
  ShipManager(string file);
  
  ~ShipManager();

  // Get access to a ship type
  Ship* newShip(unsigned int index, string shipTitle);

  static void initShipList(string file);
  static void deleteShipList();

  static ShipManager *list;

 private:
  vector<ShipInfo *> ships;

  void LoadXMLFile(string file);
};

#endif
