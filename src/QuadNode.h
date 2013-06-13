/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _QUADNODE_H_ 
#define _QUADNODE_H_

#include <GL/gl.h>

// So I don't have to include the headers
class Terrain;
class BoundBox;
class Frustum;
class WaveEmitter;

class QuadNode {
public:
  QuadNode(Terrain *terrain, BoundBox *box);
  ~QuadNode();

  // Check visibility and either draw children, or call list
  void draw(Frustum *frustum);

  // Dump info
  void dump();

  static int quadSize;

private:
  // Child nodes
  QuadNode *child1;
  QuadNode *child2;
  QuadNode *child3;
  QuadNode *child4;

  // Coords of this node
  BoundBox *nodeBox;

  // Count of vertices in index buffer
  int vertexCount;
  
  //Range of indices in the index buffer
  unsigned int indexStartRange;
  unsigned int indexEndRange;

  // Actual index buffer used to draw vertex arrays
  unsigned int *indexArray;

  // Emitter for all waves in this box, this should only be set if leaf node
  WaveEmitter *emitter;
};

#endif
