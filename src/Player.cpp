/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdio>
#include <vector>
#include <GL/gl.h>
#include "Player.h"
#include "Log.h"
#include "Ships.h"
#include "Economy.h"
#include "tinyxml.h"

using namespace std;

Player::Player() {
  x = 0.0;
  z = 0.0;
  rot = 0.0;
  speed = 0.0;

  gold = 100;

  ship = ShipManager::list->newShip(0, "Player");
}

Player::~Player() {
  delete ship;
}

bool Player::load(int number) {
  GLfloat tx, tz, trot, tspeed;
  int tdamage;
  int n;

  TiXmlDocument doc("./LSave.xml");
  TiXmlElement* Lephisto;
  TiXmlElement* SavedGame;
  TiXmlElement* Location;
  TiXmlElement* loc;
  TiXmlElement* Direction;
  TiXmlElement* Speed;
  TiXmlElement* Damage;
  TiXmlText* data;

  
  doc.LoadFile();
  if(doc.Error()) {
    Log::logger->message("\nError: Could not open saved game file\n");
    return false;
  }

  Lephisto = doc.FirstChildElement("Lephisto");
  if(Lephisto == NULL) {
    Log::logger->message("\nError: <Lephisto> did not parse correctly.\n");
    return false;
  }

  SavedGame = Lephisto->FirstChildElement("SavedGame");
  if(SavedGame == NULL) {
    Log::logger->message("\nError: <SavedGame> did not parse correctly.\n");
    return false;
  }

  try {
    Location = SavedGame->FirstChildElement("Location");
    if(Location == NULL) throw 0;

    // Handle location
    loc = Location->FirstChildElement("x");
    if(loc == NULL) throw 1;
    data = loc->FirstChild()->ToText();
    if(data == NULL) throw 1;
    n = sscanf(data->Value(), "%f", &tx);;
    if(n != 1) throw 1;
    loc = Location->FirstChildElement("z");
    if(loc == NULL) throw 2;
    data = loc->FirstChild()->ToText();
    if(data == NULL)  throw 2;
    n = sscanf(data->Value(), "%f", &tz);
    if(n != 1) throw 2;

    // Handle the direction
    Direction = SavedGame->FirstChildElement("Direction");
    if(Direction == NULL) throw 3;
    data = Direction->FirstChild()->ToText();
    if(data == NULL) throw 3;
    n = sscanf(data->Value(), "%f", &trot);
    if(n != 1) throw 3;

    // Handle speed
    Speed = SavedGame->FirstChildElement("Speed");
    if(Speed == NULL) throw 4;
    data = Speed->FirstChild()->ToText();
    if(data == NULL) throw 4;
    n = sscanf(data->Value(), "%f", &tspeed);
    if(n != 1) throw 4;

    // Deal with ship
    // Idealy this would call a method of the ship object that would get itself loaded from XML
    // that could also call a method on the cargo object that loads itself. However, for now, just saving the damage will do.
    Damage = SavedGame->FirstChildElement("Damage");
    if(Damage == NULL) throw 5;
    data = Damage->FirstChild()->ToText();
    if(data == NULL) throw 5;
    n = sscanf(data->Value(), "%d", &tdamage);
    if(n != 1) throw 5;
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("\nWarning: <Location> did not parse correctly\n");
      break;
    case 1:
      Log::logger->message("\nWarning: <x> did not parse correctly\n");
      break;
    case 2:
      Log::logger->message("\nWarning: <z> did not parse correctly\n");
      break;
    case 3:
      Log::logger->message("\nWarning: <Direction> did  not parse correctly\n");
      break;
    case 4:
      Log::logger->message("\nWarning: <Speed> did not parse correctly");
      break;
    case 5:
      Log::logger->message("\nWarning: <Damage> did not parse correctly\n");
      break;
    default:
      Log::logger->message("\nWarning: Unknown Saved Game.. Error\n");
      break;
    }

    return false;
  }

  x = tx;
  z = tz;
  rot = trot;
  speed = tspeed;
  ship->damage = tdamage;

  return true;

}

bool Player::save(int number) {
  int n;
  TiXmlDocument doc;
  TiXmlElement* Lephisto  = new TiXmlElement("");
  TiXmlElement* SavedGame = new TiXmlElement("");
  TiXmlElement* Location  = new TiXmlElement("");
  TiXmlElement* locX      = new TiXmlElement("");
  TiXmlElement* locZ      = new TiXmlElement("");
  TiXmlElement* Direction = new TiXmlElement("");
  TiXmlElement* Speed     = new TiXmlElement("");
  TiXmlElement* Damage    = new TiXmlElement("");
  TiXmlText* data         = new TiXmlText("");
  char temp[80]           = ""; // Needs to have a size otherwise we will get a buffer overflow

  //Lephisto = doc.RootElement();

  Lephisto->SetValue("Lephisto");

  SavedGame->SetValue("SavedGame");
  Location->SetValue("Location");
  locX->SetValue("x");
  locZ->SetValue("z");
  
  n = sprintf(temp, "%f", z);
  data->SetValue(temp);
  locZ->InsertEndChild(*data);

  Location->InsertEndChild(*locX);
  Location->InsertEndChild(*locZ);

  Direction->SetValue("Direction");
  n = sprintf(temp, "%f", rot);
  data->SetValue(temp);
  Direction->InsertEndChild(*data);

  Speed->SetValue("Speed");
  n = sprintf(temp, "%f", speed);
  data->SetValue(temp);
  Speed->InsertEndChild(*data);

  Damage->SetValue("Damage");
  n = sprintf(temp, "%d", ship->damage);
  data->SetValue(temp);
  Damage->InsertEndChild(*data);

  SavedGame->InsertEndChild(*Location);
  SavedGame->InsertEndChild(*Direction);
  SavedGame->InsertEndChild(*Speed);
  SavedGame->InsertEndChild(*Damage);
  

  Lephisto->InsertEndChild(*SavedGame);
  
  doc.InsertEndChild(*Lephisto);
  
  doc.SaveFile(".LSave.xml");

  return true;
}

void Player::reset() {
  x = 0.0;
  z = 0.0;
  rot = 0.0;
  speed = 0.0;
}

void Player::dump() {
  Log::logger->message("X: %f Z:%f\nRot: %f\nSpeed: %f\n\n", x, z, rot, speed);

  ship->dump();

  Log::logger->message("Gold:\t%i\n", gold);
}

Cargo* Player::getCargo() {
  return ship->cargo;
}
