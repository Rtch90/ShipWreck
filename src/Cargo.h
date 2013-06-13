/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _CARGO_H_
#define _CARGO_H_

#include <vector>
#include <string>

class Product;

using namespace std;

class Cargo {
public:
  Cargo(string nameIn, unsigned int sizeIn);
  ~Cargo();

  unsigned int getFilled();
  unsigned int getSize();
  void setSize(unsigned int sizeIn);
  unsigned int getAvailQty();

  unsigned int getQty(unsigned int product) throw ( const char *);
  void setQty(unsigned int product, unsigned int qty) throw ( const char *);
  unsigned int addQty(unsigned int product, int qty) throw ( const char *);

  void fillRandom() throw (const char*);
  void fillRandom(unsigned int qty) throw ( const char *);

  void dump();

private:
  vector<int> products;
  
  unsigned int size;
  unsigned filled;

  string name;
};

#endif