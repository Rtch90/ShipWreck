/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */


#ifndef _TERRAIN_H_ 
#define _TERRAIN_H_

#define DETAIL_SCALE .15

#include "QuadNode.h"

#include <GL/gl.h>
#include <list>

using namespace std;

// So I don't have to include the headers
class Ocean;
//class QuadNode;
class BoundBox;
class Frustum;
class Point;

class Terrain {
  // Allow QuadNodes to be friends
  friend class QuadNode;
public:
  Terrain(const char *file, GLfloat vScale, int quadSizeIn);
  ~Terrain();

  void update(unsigned int ticks);

  void draw();

  // Check a line to see if it collided with the terrain around it
  bool checkCollision_Line(GLfloat xIn, GLfloat zIn, int size, double v[3], float mag);

  // Get the height of the terrain at a specific location
  GLfloat getHeight(int x, int y);

  // Calc height at any point (doesn't need to be exactly on a vertex)
  GLfloat calcHeight(GLfloat x, GLfloat z);

  // Return w or h
  int getWidth();
  int getHeight();

  // For use with the quadtree, generate the index array for this box. min/maxHeight allow the return of min/max height of terrain in that box
  // it is the caller's responsibility to delete the array that is returned.
  unsigned int *genIndexArray(BoundBox *box, int *vertexCount, GLfloat *maxHeight, GLfloat *minHeight, unsigned int *minRange, unsigned int *maxRange);

  // Get min/max height of terrain in the box
  void getMinMax(BoundBox *box, GLfloat *minHeight, GLfloat *maxHeight);

  void locateShoreLine(BoundBox *box, list< Point* > *pointList);
  bool findNearestShoreAngle(Point *p, float *angle);

private:
  GLfloat *terrain;  // should be w*h
  GLfloat *normals;  // Should be w*h*3

  // This is the array used to draw the terrain
  // Note: This array interleaves vertex (3), normal (3), texture coord 0 (2), and texture coord 1 (2) data,
  // In other words, it should be: w*h*(3+3+2+2)
  GLfloat *terrainArray;
  
  // This is just a pointer to wherever the correct data for collisions, etc is located
  // If Vertex buffer objects are used, terrainArray gets deleted and this needs to be created as a seperate array
  // if VA's are used, this can just point to terrainArray.
  GLfloat *vertexData;

  // This is used for the collision code to index into the vertexData array correctly
  // If Vertex buffer objects are used, this will be 1, otherwise it will be (3+3+2+2)
  int vertexStride;

  // This is used for the collision code to index into the vertexData array correctly
  // If vertex buffer objects are used, this will be 0, with vertex arrays it will be 1.
  int vertexOffset;

  // Handle the vertex buffer object
  GLuint buffer;

  // Ocean surfaced object
  Ocean *ocean;

  // Width (# of pixels) of heightmap
  int w;
  // Height (# of pixels) of heightmap
  int h;

  // For image
  GLuint texture;
  // For detail texture
  GLuint detailTex;

  // Material
  GLfloat material[4];

  // Root of quadtree
  QuadNode *root;

  // Frustum used for culling quadtree
  Frustum *frustum;

  bool needCollision(int x, int z);

  // Calculate normals for the terrain and store them in terrainArray
  void calcNormals();
  void calcNormals2(char *file);

  // This function calls glVertexPointer, to initialize vertex arrays
  void setGlArrayPointers();
};

#endif
