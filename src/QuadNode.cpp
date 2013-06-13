/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#include <iostream>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <GL/glu.h>
#include "Terrain.h"
#include "BoundBox.h"
#include "Point.h"
#include "Frustum.h"
#include "extensions.h"
#include "WaveEmitter.h"
#include "Ocean.h"
#include "QuadNode.h"

int QuadNode::quadSize = 40;

using namespace std;

QuadNode::QuadNode(Terrain *terrain, BoundBox *box) {
  GLfloat xSize, zSize;
  GLfloat tHigh, tLow;

  indexStartRange = 0;
  indexEndRange   = 0;

  child1 = NULL;
  child2 = NULL;
  child3 = NULL;
  child4 = NULL;

  nodeBox = box;

  xSize = nodeBox->b2->x - nodeBox->b1->x;
  zSize = nodeBox->b3->z - nodeBox->b1->z;

  if((xSize < quadSize) && (zSize < quadSize)) {
    // Small enough to be a leaf. :)
    indexArray = terrain->genIndexArray(nodeBox, &vertexCount, &tLow, &tHigh, &indexStartRange, &indexEndRange);
    
    nodeBox->setHeight(tLow, tHigh);

    emitter = new WaveEmitter(terrain, nodeBox);
    if(emitter->numWaveLocations() > 0) {
      // No sense in bothering regestering it with the ocean object if it has no shore in it
      terrain->ocean->addEmitter(emitter);
    }
  } else {
    // Need to break into children

    // Make sure indexArray and emitter are NULL
    indexArray = NULL;
    emitter    = NULL;

    // Set up its max/min height
    terrain->getMinMax(nodeBox, &tLow, &tHigh);
    nodeBox->setHeight(tLow, tHigh);

    // Child boxes
    BoundBox *child1Box;
    BoundBox *child2Box;
    BoundBox *child3Box;
    BoundBox *child4Box;

    // Set up child1
    child1Box = new BoundBox();
    child1Box->b1 = new Point(nodeBox->b1);

    child1Box->b2 = new Point();
    Point::average(nodeBox->b1, nodeBox->b2, child1Box->b2);

    child1Box->b3 = new Point();
    Point::average(nodeBox->b1, nodeBox->b3, child1Box->b3);

    child1Box->b4 = new Point();
    Point::average(nodeBox->b1, nodeBox->b4, child1Box->b4);

    child1 = new QuadNode(terrain, child1Box);

    // Set up child2
    child2Box = new BoundBox();
    child2Box->b1 = new Point(child1Box->b2);
    child2Box->b2 = new Point(nodeBox->b2);
    child2Box->b3 = new Point(child1Box->b4);

    child2Box->b4 = new Point();
    Point::average(nodeBox->b2, nodeBox->b4, child2Box->b4);

    child2 = new QuadNode(terrain, child2Box);

    // Set up child3
    child3Box = new BoundBox();
    child3Box->b1 = new Point(child1Box->b3);
    child3Box->b2 = new Point(child1Box->b4);
    child3Box->b3 = new Point(nodeBox->b3);

    child3Box->b4 = new Point();
    Point::average(nodeBox->b3, nodeBox->b4, child3Box->b4);

    child3 = new QuadNode(terrain, child3Box);

    // Set up child4
    child4Box = new BoundBox();
    child4Box->b1 = new Point(child1Box->b4);
    child4Box->b2 = new Point(child2Box->b4);
    child4Box->b3 = new Point(child3Box->b4);
    child4Box->b4 = new Point(nodeBox->b4);

    child4 = new QuadNode(terrain, child4Box);

    // All of those child boxes get handled (deleted) by the children
  }
}

QuadNode::~QuadNode() {
  delete nodeBox;

  // If it's a leaf, delete the index array
  if(indexArray != NULL) {
    delete []indexArray;
    indexArray = NULL;
  }

  if(emitter != NULL) {
    delete emitter;
  }

  delete child1;
  delete child2;
  delete child3;
  delete child4;
}

void QuadNode::draw(Frustum *frustum) {
  bool visible;

  // Check to see if it's visible
  visible = frustum->checkBox(nodeBox);
  if(!visible) return;
  if(indexArray != NULL) {
    // This is a leaf, draw it
    glDrawRangeElements(GL_TRIANGLE_STRIP, indexStartRange, indexEndRange, vertexCount, GL_UNSIGNED_INT, indexArray);
    
    // Set the emitter as visible
    emitter->setVisible();

    //glDisable(GL_FOG);
    //nodeBox->draw();
    //glEnable(GL_FOG);
    return;
  } else {
    // It must be visible and not a leaf, so try the children
    //nodeBox->draw();

    if(child1 != NULL) child1->draw(frustum);
    if(child2 != NULL) child2->draw(frustum);
    if(child3 != NULL) child3->draw(frustum);
    if(child4 != NULL) child4->draw(frustum);
  }
}

void QuadNode::dump() {
  if(indexArray != NULL) {
    printf("Leaf Node (Vertex Count %d):\n", vertexCount);
  } else {                      
    printf("Non-leaf Node:\n");
  }
  nodeBox->b1->dump();
  nodeBox->b2->dump();
  nodeBox->b3->dump();
  nodeBox->b4->dump();

  nodeBox->t1->dump();
  nodeBox->t2->dump();
  nodeBox->t3->dump();
  nodeBox->t4->dump();
}
