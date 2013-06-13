/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _WAVEEMITTER_H_
#define _WAVEEMITTER_H_

#include <vector>
#include <list>

using namespace std;

#define NUM_WAVES 5

class Wave;
class Terrain;
class BoundBox;
class Point;
struct WaveLoc;

class WaveEmitter {
public:
  WaveEmitter(Terrain *terrain, BoundBox *box);
  
  ~WaveEmitter();

  void update(unsigned int ticks);
  
  void setVisible();

  int numWaveLocations();

  static void drawActiveWaves();
  static void updateActiveWaves(unsigned int ticks);
  static void deleteActiveWaves();

  static void generateWaveList();
  static void deleteWaveList();

private:
  // Surf display list
  static GLuint waveList;

  // Number of ticks until next emit
  int ticksToNextEmit;

  // Active waves
  vector< WaveLoc* > waveLocations;

  // Whether this area is visible or not
  bool isVisible;

  // Static list of all the waves
  static list < Wave* > activeWaves;

  // Generate ticks to next emit using the random percent difference
  int getNextEmit();

  // Pick a random wave location and add it to the static wave array
  void generateWave();
};

#endif
 
