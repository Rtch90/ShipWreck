/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <vector>
#include <string>
#include "Log.h"
#include "ccmath.h"
#include "Economy.h"
#include "Cargo.h"

Cargo::Cargo(string nameIn, unsigned int sizeIn) {
  size   = sizeIn;
  filled = 0;
  name   = nameIn;

  products.resize(Economy::Econ->ProductCount());

  for(unsigned int i = 0; i < products.size(); i++) {
    products[i] = 0;
  }
}

Cargo::~Cargo() {
  // Nothing realy needs to happen here
  products.clear();
}

unsigned int Cargo::getFilled() {
  return filled;
}

unsigned int Cargo::getSize() {
  return size;
}

void Cargo::setSize(unsigned int sizeIn) {
  size = sizeIn;
}

unsigned int Cargo::getAvailQty() {
  return size - filled;
}

unsigned int Cargo::getQty(unsigned int product) throw ( const char *) {
  if(product >= products.size()) {
    throw "Invalid product";
  }

  return products[product];
}

void Cargo::setQty(unsigned int product, unsigned int qty) throw ( const char *) {
  unsigned int oldQty;

  if(product >= products.size()) {
    throw "Invalid product";
  }
  
  oldQty = products[product];
  products[product] = qty;
  
  filled = filled + (qty - oldQty);
}

unsigned int Cargo::addQty(unsigned int product, int qty) throw ( const char *) {
  if(product >= products.size()) {
    throw "Invalid product";
  }
  if((qty < 0 && (-1*qty > products[product]))) {
    throw "Result would be negative";
  }

  products[product] = products[product] + qty;
  filled += qty;

  return products[product];
}

void Cargo::fillRandom() throw ( const char*) {
  fillRandom((int) (size * randBellCurve()));
}

void Cargo::fillRandom(unsigned int qty) throw(const char *) {
  unsigned int avgQty;
  unsigned int tempQty;

  if(qty > size) {
    throw "Qty greater than size";
  }

  avgQty = qty / products.size();
  Log::logger->message("AvgQty: %u", avgQty);

  // Loop through all but last
  for(unsigned int i = 0; i < (products.size() - 1); i++) {
    tempQty = (int) ((randBellCurve() + .5) * avgQty);
    if(tempQty > getAvailQty()) {
      tempQty = getAvailQty();
    }

    products[i] = tempQty;

    filled += tempQty;

    // If it's full, there is no sence to keep going on
    if(getAvailQty() == 0) {
      break;
    }
  }

  products[products.size()-1] = qty - filled;
  filled = qty;
}

void Cargo::dump() {
  Log::logger->message("%s", name.c_str());
  Log::logger->message("Size:\t%u", size);
  Log::logger->message("Filled:\t%u", filled);

  for(unsigned int i = 0; i < products.size(); i++) {
    Log::logger->message("%s:\t%u", Economy::Econ->ProdList[i].c_str(), products[i]);
  }
}
