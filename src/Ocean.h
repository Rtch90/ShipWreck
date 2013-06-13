/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _OCEAN_H_
#define _OCEAN_H_

#include <vector>

using namespace std;

class WaveEmitter;

class Ocean {
public:
  // Constructor
  Ocean(int sizeX, int sizeZ);

  // Destructor
  ~Ocean();

  // Draw the actual water
  void draw();

  // Update the waters position
  void update(unsigned int ticks);

  // Add a WaveEmitter to the master list
  void addEmitter(WaveEmitter *emitter);
  
private:
  // Size of water patch
  int width;
  int height;

  // Texture handle
  GLuint texture;

  // Texture handle for waves
  GLuint waveTexture;

  // Material
  GLfloat material[4];

  // For sin/cos to move texture
  GLfloat count;

  // For display list
  GLuint list;

  // Pointer to all of the wave emitters (This makes update easier)
  vector< WaveEmitter* > waveEmitters;
};

#endif
