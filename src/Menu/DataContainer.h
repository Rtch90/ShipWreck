/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 *
 * This class is the parent of the button class. It is
 * what text or data is displayed with. It is also what
 * the button will derive.
 */

#ifndef _DATACONTAINER_H_
#define _DATACONTAINER_H_

#include <string>

using namespace std;

class DataContainer {
public:
  // Constructor
  DataContainer(string Text);
  DataContainer(string Text, float red, float green, float blue);
  
  // Destructor
  virtual ~DataContainer();
  
  // Methods
  void ChangeText(string Text);
  string GetText();

  void Display();
  
  void SetColor(float red, float green, float blue);

  int GetLength();
    
  virtual bool Selectable();

  virtual void Print();

protected:
  string myText;
  
  float redColor;
  float greenColor;
  float blueColor;

  bool amISelectable; // ???????????

private:

};

#endif
