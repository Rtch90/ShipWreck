/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "extensions.h"

#include <GL/gl.h>
//#include <GL/glext.h>
#include <SDL/SDL_image.h>
//#define _USE_MATH_DEFINES - I do not think this should be needed, if it is in windows, put it in the windows pre-processor.
#include <iostream>
#include <list>
#include "ccmath.h"
#include "Log.h"
#include "Config.h"
#include "image.h"
#include "normals.h"
#include "collisions.h"
#include "QuadNode.h"
#include "BoundBox.h"
#include "Point.h"
#include "Frustum.h"
#include "Ocean.h"
#include "Terrain.h"

using namespace std;

// Used to get index in terrain array based on (x, z) coords
//#define TERRAIN(X, Z) terrain[((Z) * w) + (X)]

// This looks up the normal at (x, z) and gets the Tth element of that point (0 = x, 1 = y, 2 = z)
//#define NORMALS(X, Z, T) normals[(((Z)*w + (X)) * 3) + (T)]

#define TNORMALS_INDEX(X, Z, T) ((((Z)*w + (X)) * 3) + (T))

#define TERRAIN_ARRAY(X, Z, T) terrainArray[((((Z) * w) + (X)) * (3+3+2+2)) + (T)]

#define TERRAIN_ARRAY_INDEX(X, Z) (((Z) * w) + (X))

#define VERTEX(X, Z) vertexData[((((Z) * w) + (X)) * vertexStride) + vertexOffset]

#define ARRAY_STRIDE ((3+3+2+2) * sizeof(GLfloat))

#define ARRAY_VERTEX 0
#define ARRAY_NORMAL 3
#define ARRAY_TEX0 6
#define ARRAY_TEX1 8

Terrain::Terrain(const char *file, GLfloat vScaleIn, int quadSizeIn) {
  SDL_Surface *Image;
  BoundBox *box;
  Uint8 *p;

  // To center the terrain
  int xDiff;
  int zDiff;
  
  // Initialize any extensions, if needs be
  initExtensions();

  QuadNode::quadSize = quadSizeIn;

  // Load the image, check for errors, if image is not found we should throw an exeption
  Image = IMG_Load(file);
  if (!Image) {
	  Log::logger->message("Warning: Could not load file %s", file);
  }

  w = Image->w;
  h = Image->h;

  xDiff = (int) w/2;
  zDiff = (int) h/2;

  // Set up the ocean
  ocean = new Ocean(w, h);
  if(ocean == NULL) {
    Log::logger->message("Warning: Could not create ocean object");
  } else {
    Log::logger->message("Ocean created");
  }

  // This is the array used to draw the terrain
  // Note: This array interleaves vertex (3), normal (3), texture coord 0 (2), and texture coord 1 (2) data
  terrainArray = new GLfloat[w*h*(3+3+2+2)]; 

  if(Config::configuration->checkVBO()) {
    // Using VBO's, so terrainArray will get uploaded to the card and deleted from system memory
    // We need a copy of the vertex data for collisions
    vertexData = new GLfloat[w*h];

    // Set the vertexStride to be 1, since the data is tightly packed and we only store the height there.
    vertexStride = 1;

    // Set the vertexOffset to be 0, since all we are storing is the height values
    vertexOffset = 0;
  } else {
    // Using vertez arrays so the terrainArray will stay valid
    vertexData = terrainArray;

    // Set the vertexStride to be (3+3+2+2), since terrainArray contains texture and normal data too.
    vertexStride = (3+3+2+2);

    // Set the vertexOffset to be 1, since the height is the second value in the array at a coord
    vertexOffset = 1;
  }

  p = (Uint8 *) Image->pixels;

  for(int z = 0; z < h; z++){
    for(int x = 0; x < w; x++){
      
      TERRAIN_ARRAY(x, z, ARRAY_VERTEX+0) = (GLfloat) x - xDiff;
      TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1) = (p[z*Image->pitch + x*Image->format->BytesPerPixel] - 128) * vScaleIn;
      TERRAIN_ARRAY(x, z, ARRAY_VERTEX+2) = (GLfloat) z - zDiff;

      // If using Vertex buffer objects, constuct the vertexData array too.
      // This is needed because terrainArray will get uploaded to the card and deleted.
      if(Config::configuration->checkVBO()) {
	VERTEX(x, z) = TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1);
      }

      // Color texture coords
      TERRAIN_ARRAY(x, z, ARRAY_TEX0+0) = ((float) x) / ((float) w);
      TERRAIN_ARRAY(x, z, ARRAY_TEX0+1) = ((float) z) / ((float) h);

      // Detail texture coords
      TERRAIN_ARRAY(x, z, ARRAY_TEX1+0) = x*DETAIL_SCALE;
      TERRAIN_ARRAY(z, z, ARRAY_TEX1+1) = z*DETAIL_SCALE;

      // the normals get calculated when calcNormals() is called
    }
  }

  SDL_FreeSurface(Image);

  Log::logger->message("Heightmap generated");

  calcNormals();
  //calcNormals2("./normalMap.png");

  Log::logger->message("Normals generated");

  //printf("%f %f %f\n", NORMALS(350, 350, 0), NORMALS(350, 350, 1), NORMALS(350, 350, 2));
  
  if(!BuildTexture("./data/land.png", &texture, GL_CLAMP_TO_EDGE, true)) {
    Log::logger->message("Warning: Could not load ./data/land.png");
  }

  if(!BuildTexture("./data/detail.png", &detailTex, GL_REPEAT, true)) {
    Log::logger->message("Warning: Could not load ./data/detail.png");
  }

  // Set up openGL vertex arrays
  setGlArrayPointers();

  // Set up the color texture
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Set up the color detail
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, detailTex);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);

  // Set up the material
  material[0] = 1.0;
  material[1] = 1.0;
  material[2] = 1.0;
  material[3] = 1.0;

  // Set up the root quadtree box
  box = new BoundBox();

  box->b1 = new Point(0.0 - (int) w/2, 0.0, 0.0 - (int) h/2);
  box->b2 = new Point((int) w/2, 0.0, 0.0 - (int) h/2);
  box->b3 = new Point(0.0 - (int) w/2, 0.0, (int) h/2 - 1);
  box->b4 = new Point((int) w/2 - 1, 0.0, (int) h/2 - 1);

  // Set up the frustum
  frustum = new Frustum();

  // Initialize the quadtree root
  root = new QuadNode(this, box);

  Log::logger->message("Quadtree generated");

  // Don't need to worry about deleting the box, the quadtree will do it

  if(Config::configuration->checkVBO()) {
    // This data is now on the card, we don't need it anymore
    delete []terrainArray;
    terrainArray = NULL;
  }

  // Disable the multi-texturing, so other constructors aren't weird
  //glActiveTexture(GL_TEXTURE1);  // Should already be selected
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
}

Terrain::~Terrain() {
  // Delete the ocean
  delete ocean;

  // Delete the quadtree
  delete root;

  // Delete the view frustum
  delete frustum;

  if(Config::configuration->checkVBO()) {
    // Free up the memory on the graphics card
    glDeleteBuffers(1, &buffer);

    // Delete the temp data used for collisions
    delete []vertexData;
  } else {
    // Vertex arrays where used, so delete all the data
    delete []terrainArray;
    terrainArray = NULL;

    // vertexData was set to terrainArray, so set it to NULL
    vertexData = NULL;
  }
 
  // Delete the textures
  glDeleteTextures(1, &texture);
  glDeleteTextures(1, &detailTex);

  // Again, still don't need to delete the constructor's box, the quadtree
  // handles it
}

void Terrain::update(unsigned int ticks) {
  ocean->update(ticks);
}

void Terrain::draw() {
  // Use the material
  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

  // Set up the color texture
  glActiveTexture(GL_TEXTURE0);
  //glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set up the color detail
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, detailTex);

  frustum->getFrustum();

  root->draw(frustum);

  // Disable the texturing
  //glActiveTexture(GL_TEXTURE1);  // Should already be selected
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  glEnable(GL_BLEND);
  ocean->draw();
  glDisable(GL_BLEND);
}

bool Terrain::checkCollision_Line(GLfloat xIn, GLfloat zIn, int size, 
				 double v[3], float mag) {
  int xMin, xMax;
  int zMin, zMax;

  xIn = (w/2) + xIn;
  zIn = (h/2) + zIn;

  xMin = (int) xIn - (size/2);
  xMax = (int) xIn + (size/2);
  if (xMin < 0) xMin = 0;
  if (xMax > (w-2)) xMax = (w-2);

  zMin = (int) zIn - (size/2);
  zMax = (int) zIn + (size/2);
  if (zMin < 0) zMin = 0;
  if (zMax > (h-2)) zMax = (h-2);
  
  for(int z = zMin; z < zMax; z++){
    for(int x = xMin; x < xMax; x++) {
      if(needCollision(x, z)) {
	double orig[3];
	double vert0[3];
	double vert1[3];
	double vert2[3];
	double vert3[3];
	double r1, r2, r3;

	orig[0] = xIn;
	orig[1] = 0;
	orig[2] = zIn;

	vert0[0] = x;
	vert0[1] = VERTEX(x, z);
	vert0[2] = z;

	vert1[0] = x+1;
	vert1[1] = VERTEX(x+1, z);
	vert1[2] = z;

	vert2[0] = x;
	vert2[1] = VERTEX(x, z+1);
	vert2[2] = z+1;

	vert3[0] = x+1;
	vert3[1] = VERTEX(x+1, z+1);
	vert3[2] = z+1;

	if(intersect_triangle(orig, v, vert0, vert2, vert1, &r1, &r2, &r3)) {
	  if(r1 < mag) return true;
	}
	if(intersect_triangle(orig, v, vert2, vert3, vert1, &r1, &r2, &r3)) {
	  if(r1 < mag) return true;
	}
      }
    }
  }

  return false;
}

bool Terrain::needCollision(int x, int z) {
  GLfloat v1, v2, v3, v4;

  v1 = VERTEX(x, z);
  v2 = VERTEX(x, z+1);
  v3 = VERTEX(x+1, z);
  v4 = VERTEX(x+1, z+1);

  if(((v1 <= 0) || (v2 <= 0) || (v3 <= 0) || (v4 <= 0)) &&
     ((v1 >= 0) || (v2 >= 0) || (v3 >= 0) || (v4 >= 0))) {
    return true;
  } else return false;
}


void Terrain::calcNormals() {
  GLfloat point1[3], point2[3], point3[3];
  GLfloat *tnormals1;
  GLfloat *tnormals2;
  // For loops, so MSVC++ doesn't complain
  short int z, x;

  if(terrainArray == NULL) return;

  tnormals1 = new GLfloat[w*h*3];
  tnormals2 = new GLfloat[w*h*3];

  for(z = 0; z < h-1; z++) {
    for(x = 0; x < w-1; x++) {
      point1[0] = x;
      point1[1] = TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1);
      point1[2] = z;

      point2[0] = x;
      point2[1] = TERRAIN_ARRAY(x, z+1, ARRAY_VERTEX+1);
      point2[2] = z+1;

      point3[0] = x+1;
      point3[1] = TERRAIN_ARRAY(x+1, z, ARRAY_VERTEX+1);
      point3[2] = z;

      crossProduct(point1, point2, point3, &tnormals1[TNORMALS_INDEX(x, z, 0)]);

      point1[0] = x;
      point1[1] = TERRAIN_ARRAY(x, z+1, ARRAY_VERTEX+1);
      point1[2] = z+1;

      point2[0] = x+1;
      point2[1] = TERRAIN_ARRAY(x+1, z+1, ARRAY_VERTEX+1);
      point2[2] = z+1;

      point3[0] = x+1;
      point3[1] = TERRAIN_ARRAY(x+1, z, ARRAY_VERTEX+1);
      point3[2] = z;

      crossProduct(point1, point2, point3, &tnormals2[TNORMALS_INDEX(x, z, 0)]);
    }
  }

  for(z = 1; z < h-1; z++) {
    for(x = 1; x < w-1; x++) {
    /*
      NORMALS(x, z, 0) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 0)] + tnormals1[TNORMALS_INDEX(x-1, z, 0)] +
			  tnormals2[TNORMALS_INDEX(x-1, z, 0)] + tnormals1[TNORMALS_INDEX(x, z-1, 0)] +
			  tnormals2[TNORMALS_INDEX(x, z-1, 0)] + tnormals1[TNORMALS_INDEX(x, z, 0)]);
      NORMALS(x, z, 1) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 1)] + tnormals1[TNORMALS_INDEX(x-1, z, 1)] + 
			  tnormals2[TNORMALS_INDEX(x-1, z, 1)] + tnormals1[TNORMALS_INDEX(x, z-1, 1)] +
			  tnormals2[TNORMALS_INDEX(x, z-1, 1)] + tnormals1[TNORMALS_INDEX(x, z, 1)]);
      NORMALS(x, z, 2) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 2)] + tnormals1[TNORMALS_INDEX(x-1, z, 2)] +
			  tnormals2[TNORMALS_INDEX(x-1, z, 2)] + tnormals1[TNORMALS_INDEX(x, z-1, 2)] +
			  tnormals2[TNORMALS_INDEX(x, z-1, 2)] + tnormals1[TNORMALS_INDEX(x, z, 2)]);
    */
      
      TERRAIN_ARRAY(x, z, ARRAY_NORMAL+0) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 0)] + tnormals1[TNORMALS_INDEX(x-1, z, 0)] +
      tnormals2[TNORMALS_INDEX(x-1, z, 0)] + tnormals1[TNORMALS_INDEX(x, z-1, 0)] +
      tnormals2[TNORMALS_INDEX(x, z-1, 0)] + tnormals1[TNORMALS_INDEX(x, z, 0)]);
      TERRAIN_ARRAY(x, z, ARRAY_NORMAL+1) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 1)] + tnormals1[TNORMALS_INDEX(x-1, z, 1)] +
      tnormals2[TNORMALS_INDEX(x-1, z, 1)] + tnormals1[TNORMALS_INDEX(x, z-1, 1)] +
      tnormals2[TNORMALS_INDEX(x, z-1, 1)] + tnormals1[TNORMALS_INDEX(x, z, 1)]);
      TERRAIN_ARRAY(x, z, ARRAY_NORMAL+2) = (tnormals2[TNORMALS_INDEX(x-1, z-1, 2)] + tnormals1[TNORMALS_INDEX(x-1, z, 2)] +
      tnormals2[TNORMALS_INDEX(x-1, z, 2)] + tnormals1[TNORMALS_INDEX(x, z-1, 2)] +
      tnormals2[TNORMALS_INDEX(x, z-1, 2)] + tnormals1[TNORMALS_INDEX(x, z, 2)]);
      
      // Send normalize pointer to first element for that normal
      //normalize(&NORMALS(x, z, 0));
      normalize(&TERRAIN_ARRAY(x, z, ARRAY_NORMAL));
    }
  }

  // Get rid of temp normal arrays
  delete []tnormals1;
  delete []tnormals2;
}

GLfloat Terrain::getHeight(int x, int y) {
  if ((x < w) && (y < h)) {
    return VERTEX(x, y);
  } else return 0; // Should throw an exeption
}

// The array that is returned by this function needs to be deleted by the caller
unsigned int *Terrain::genIndexArray(BoundBox *box, int *vertexCount, GLfloat *minHeight, GLfloat *maxHeight, unsigned int *minRange, unsigned int *maxRange) {
  // Used to find the min/max height on this quad box

  GLfloat tMaxHeight = -256;
  GLfloat tMinHeight =  256;

  unsigned int tMinRange = w*h+1;
  unsigned int tMaxRange = 0;

  // Used to go from centered coords to array indicies
  int xDiff = (int) w/2;
  int zDiff = (int) h/2;

  // Find min/max x coords in 0 to w range (instead of -w/e to w/2)
  int xMin = ((int) box->b1->x) + xDiff;
  int xMax = ((int) box->b2->x) + xDiff;

  // Find min/max z coords in 0 to h range (instead of -h/2 to h/2)
  int zMin = ((int) box->b1->z) + zDiff;
  int zMax = ((int) box->b3->z) + zDiff;

  // These are used to allow generation of a single triangle strip, it works one direction, then back the other, untill the tile is drawn :)
  int step = -1;
  int xEnd;
  int x;

  int count = 0;

  
  // These are used to compute the number of vertices in this index array.
  int xCount;
  int zCount;


  /*
   * Long ago, in a finite state far away, there lived a JOVIAL
   * character named Jack.  Jack and his relations were poor.  Often their
   * hash table was bare.  One day Jack's parent said to him, "Our matrices
   * are sparse.  You must go to the market to exchange our RAM for some
   * BASICs."  She compiled a linked list of items to retrieve and passed it
   * to him.
   * So Jack set out.  But as he was walking along a Hamilton path,
   * he met the traveling salesman.
   * "Whither dost thy flow chart take thou?" prompted the salesman
   * in high-level language.
   * "I'm going to the market to exchange this RAM for some chips
   * and Apples," commented Jack.
   * "I have a much better algorithm.  You needn't join a queue
   * there; I will swap your RAM for these magic kernels now."
   * Jack made the trade, then backtracked to his house.  But when
   * he told his busy-waiting parent of the deal, she became so angry she
   * started thrashing.
   * "Don't you even have any artificial intelligence?  All these
   * kernels together hardly make up one byte," and she popped them out the
   * window...
   * :D
   */
 

  // Set the xCount and zCount
  xCount = (int) box->b2->x - (int) box->b1->x + 1;
  if (zMax >= h) {
    // The last row won't get drawn (z+1 would be out of bounds)
    zCount = (int) box->b3->z - (int) box->b1->z - 1;
  } else {
    // Need to account for the last row
    zCount = (int) box->b3->z - (int) box->b1->z;
  }

  // Calculate the actual number of verticies
  *vertexCount = (xCount * 2 + 1) * zCount;

  unsigned int *indexArray = new unsigned int[*vertexCount];

  // may print zMin and zMax or something.

  // Generate the actual index array
  for(int z = zMin; z < zMax; z++) {
    //The step allows generation of a single triangle strip, we need to work across one, then back the next
    step = -1 * step;
    if(step > 0) {
      x = xMin;
      xEnd = xMax+1;
    } else {
      x = xMax;
      xEnd = xMin-1;
    }

    // Make sure z+1 is in bounds
    if(z+1 < h) {
      // It is, so loop through and drae this section of the strip
      while(x != xEnd) {
        // Check to see if we need to update min/maxHeight
        if(getHeight(x, z) > tMaxHeight) tMaxHeight = getHeight(x, z);
        if(getHeight(x, z) < tMinHeight) tMinHeight = getHeight(x, z);

        if(getHeight(x, z+1) > tMaxHeight) tMaxHeight = getHeight(x, z+1);
        if(getHeight(x, z+1) < tMinHeight) tMinHeight = getHeight(x, z+1);

        // Actualy add these two points, incrementing count as you insert
        indexArray[count++] = TERRAIN_ARRAY_INDEX(x, z);
        indexArray[count++] = TERRAIN_ARRAY_INDEX(x, z+1);

        if((unsigned int) TERRAIN_ARRAY_INDEX(x, z) > tMaxRange) tMaxRange = TERRAIN_ARRAY_INDEX(x, z);
        if((unsigned int) TERRAIN_ARRAY_INDEX(x, z+1) > tMaxRange) tMaxRange = TERRAIN_ARRAY_INDEX(x, z+1);

        if((unsigned int) TERRAIN_ARRAY_INDEX(x, z) < tMinRange) tMinRange = TERRAIN_ARRAY_INDEX(x, z);
        if((unsigned int) TERRAIN_ARRAY_INDEX(x, z+1) < tMinRange) tMinRange = TERRAIN_ARRAY_INDEX(x, z+1);

        // Increment (or decrement) x
        x += step;
      }

      // Add an extra point so that even with degenerate triangles openGL faces the polys the right direction
      indexArray[count++] = TERRAIN_ARRAY_INDEX(x-step, z+1);
    }
  }

  // Set the max/min to the values found
  *maxHeight = tMaxHeight;
  *minHeight = tMinHeight;

  // Set the range to the values we found
  *minRange = tMinRange;
  *maxRange = tMaxRange;

  return indexArray;
}

void Terrain::setGlArrayPointers() {
  if(Config::configuration->checkVBO()) {
    // Using Vertex buffer objects

    // Generate the buffer object
    glGenBuffers(1, &buffer);
    
    // Enable all of the arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Bind the buffer object and copy the data to the video card
    glBindBuffer(GL_ARRAY_BUFFER_ARB, buffer);
    glBufferData(GL_ARRAY_BUFFER_ARB, w*h*(3+3+2+2)*sizeof(GL_FLOAT), terrainArray, GL_STATIC_DRAW_ARB);

    // This is quite a nasty hack to get offsets instead of pointers
    char *t = NULL;

    // Locations of vertex data in the array
    glVertexPointer(3, GL_FLOAT, ARRAY_STRIDE, &t[ARRAY_VERTEX*sizeof(GL_FLOAT)]);

    // Location of normal data
    glNormalPointer(GL_FLOAT, ARRAY_STRIDE, &t[ARRAY_NORMAL*sizeof(GL_FLOAT)]);

    // Enable Texture0 and give its locations in the array
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_FLOAT, ARRAY_STRIDE, &t[ARRAY_TEX0*sizeof(GL_FLOAT)]);

    // Enable TEXTURE1 and give its locations
    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(2, GL_FLOAT, ARRAY_STRIDE, &t[ARRAY_TEX1*sizeof(GL_FLOAT)]);
  } else {
    // We must be using Vertex arrays

    // Enable arrays and give data locations
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, ARRAY_STRIDE, &TERRAIN_ARRAY(0, 0, ARRAY_VERTEX));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, ARRAY_STRIDE, &TERRAIN_ARRAY(0, 0, ARRAY_NORMAL));

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, ARRAY_STRIDE, &TERRAIN_ARRAY(0, 0, ARRAY_TEX0));

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, ARRAY_STRIDE, &TERRAIN_ARRAY(0, 0, ARRAY_TEX1));
  }
}

void Terrain::getMinMax(BoundBox *box, GLfloat *minHeight, GLfloat *maxHeight) {
  // High and low values for this chunk of terrain
  GLfloat tHigh = 0;
  GLfloat tLow = 0;

  // To center the terrain             
  int xDiff = (int) w / 2;
  int zDiff = (int) h / 2;

  // For for loops
  int startX = ((int) box->b1->x) + xDiff;
  int endX = ((int) box->b2->x) + xDiff;
  int startZ = ((int) box->b1->z) + zDiff;
  int endZ = ((int) box->b3->z) + zDiff;

    // Step through the terrain to find high and low points
  for(int z = startZ; z < endZ; z++) {
    for(int x = startX; x < endX; x++) {
      //if(TERRAIN(x, z) > tHigh) tHigh = TERRAIN(x, z);
      //if(TERRAIN(x, z) < tLow) tLow = TERRAIN(x, z);
      if(VERTEX(x, z) > tHigh) tHigh = VERTEX(x, z);
      if(VERTEX(x, z) < tLow)  tLow  = VERTEX(z, z);
    }
  }

  // Finish up
  *maxHeight = tHigh; 
  *minHeight = tLow;
}

GLfloat Terrain::calcHeight(GLfloat x, GLfloat z) {
  // The idea here is that I cast a ray down from above the terrain using the
  //   collision checking code to figure out the exact height at any point
  int xCoord = (int) x;
  int zCoord = (int) z;

  if((x >= w) || (z >= h)) {
    // Should throw an exeption
    Log::logger->message("Warning: x (%d) or z (%d) too large in calcHeight", x, z);
    return 0;
  }
  if((x < 0) || (z < 0)) {
    // Should throw an exeption
    Log::logger->message("Warning: x (%d) or z (%d) less than zero in calcHeight", x, z);
    return 0;
  }

  double orig[3]  = {x, 128, z};  // Point we're casting from
  double ray[3]   = {0, -1, 0};  // Direction of ray
  double vert0[3] = {xCoord,   VERTEX(xCoord, zCoord), zCoord};
  double vert1[3] = {xCoord+1, VERTEX(xCoord+1, zCoord), zCoord};
  double vert2[3] = {xCoord,   VERTEX(xCoord, zCoord+1), zCoord+1};
  double vert3[3] = {xCoord+1, VERTEX(xCoord+1, zCoord+1), zCoord+1};
  double r1, r2, r3;
  
  if(intersect_triangle(orig, ray, vert0, vert2, vert1, &r1, &r2, &r3)) {
    return 128 - r1;
  }
  if(intersect_triangle(orig, ray, vert2, vert3, vert1, &r1, &r2, &r3)) {
    return 128 - r1;
  }
  printf("We have some problems!\n");
  // Shoulden't ever get here because the ray should hit at least one of the triangles
  // Probably should throw an exception
  return 0;
}

int Terrain::getWidth() {
  return w;
}

int Terrain::getHeight() {
  return h;
}

void Terrain::calcNormals2(char *file) {
  SDL_Surface *Image;
  Uint8 *p;

  // Load the image, check for errors, if image is not found quit
  Image = IMG_Load(file);
  if(!Image) {
    // Should throw an exception
    cout << "Could not load \"" << file << "\"" << endl;
  }

  // Make sure same size image
  if((Image->w != w) || (Image->h != h)) {
    // Should throw an exception
    cout << "Normal map and heightmap not the same size" << endl;
  }


  if(terrainArray == NULL) return;

  p = (Uint8 *) Image->pixels;

  for(int y = 0; y < Image->h; y++){
    for(int x = 0; x < Image->w; x++){
      //NORMALS(x, y, 0) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y] / 255) * 2 - 1;
      //NORMALS(x, y, 1) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y + 1] / 255) * 2 - 1;
      //NORMALS(x, y, 2) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y + 2] / 255) * 2 - 1;
      
      TERRAIN_ARRAY(x, y, ARRAY_NORMAL+0) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y] / 255) * 2 - 1;
      TERRAIN_ARRAY(x, y, ARRAY_NORMAL+1) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y + 1] / 255) * 2 - 1;
      TERRAIN_ARRAY(x, y, ARRAY_NORMAL+2) = ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y + 2] / 255) * 2 - 1;
      //printf("%f\n", ((double) p[(y*Image->w + x)*Image->format->BytesPerPixel + 2*y] / 255) * 2 - 1);
      
      // Send normalize pointer to first element for that normal
      //normalize(&NORMALS(x, y, 0));
      normalize(&TERRAIN_ARRAY(x, y, ARRAY_NORMAL));
    }
  }

  // Get rid of temp normal arrays
  SDL_FreeSurface(Image);
}

/* The idea of this function is that it figures out where all the points
 * that are on the shore and in the bounding box are.
 * Each wave emitter calls this on its box to get its list of points.
 */
  void Terrain::locateShoreLine(BoundBox *box, list< Point* > *pointList) {
  bool insert;

  // To center the terrain
  int xDiff = (int) w/2;
  int zDiff = (int) h/2;

  // For the for loops
  int startX = ((int) box->b1->x) + xDiff;
  int endX   = ((int) box->b2->x) + xDiff;
  int startZ = ((int) box->b1->z) + zDiff;
  int endZ   = ((int) box->b2->z) + zDiff;

  if(startX < 1) startX = 1;
  if(endX > w-2) endX   = w-2;
  if(startZ < 1) startZ = 1;
  if(endZ > h-2) endZ   = h-2;

  // Step through the points in the box looking for the shore
  for(int z = startZ; z < endZ; z++) {
    for(int x = startX; x < endX; x++) {
      
      // Reset insert variable
      insert = false;
      
      // Make sure it's in the water, no sense in checking on land
      if(TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1) > 0) continue;

      if(TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1) == 0) insert = true;
      
      if(TERRAIN_ARRAY(x+1, z, ARRAY_VERTEX+1) > 0) insert = true;
      else if(TERRAIN_ARRAY(x-1, z, ARRAY_VERTEX+1) > 0) insert = true;
      else if(TERRAIN_ARRAY(x, z+1, ARRAY_VERTEX+1) > 0) insert = true;
      else if(TERRAIN_ARRAY(x, z-1, ARRAY_VERTEX+1) > 0) insert = true;

      // Don't check diagonally, it gives weird right angles      

      if(insert) {
        pointList->push_back(new Point(x-xDiff, 0, z-zDiff));
      }
    }
  }
}

bool Terrain::findNearestShoreAngle(Point *p, float *angle) {
  int dX = w/2;
  int dZ = h/2;

  int pX = (int)p->x+dX;
  int pZ = (int)p->z+dZ;

  double orig[3] = {pX, 0, pZ};  // Point we're casting from
  double ray[3];  // Direction of ray
  double vert0[3];
  double vert1[3];
  double vert2[3];
  double vert3[3];
  double r1, r2, r3;

  int minAngle = 0;
  double minDX;
  double minDZ;
  double minDist = 100.0;

  double tempAngleRadians;

  float height = calcHeight(pX, pZ);
  if(height > -.0001) return false;

  // Cast a ray in all 360 degrees, see which one hits the land closest
  for(int tempAngle = 0; tempAngle < 360; tempAngle++) {
    tempAngleRadians = degreesToRadians(tempAngle);
    ray[0] = sin(tempAngleRadians);
    ray[1] = 0;
    ray[2] = cos(tempAngleRadians);

    // Only check within 1 in each direction, should be closer than that
    for(int z = pZ-1; z < pZ+1; z++) {
      for(int x = pX-1; x < pX+1; x++) {
        vert0[0] = x;
        vert0[1] = TERRAIN_ARRAY(x, z, ARRAY_VERTEX+1);
        vert0[2] = z;

        vert1[0] = x+1;
        vert1[1] = TERRAIN_ARRAY(x+1, z, ARRAY_VERTEX+1);
        vert1[2] = z;

        vert2[0] = x;
        vert2[1] = TERRAIN_ARRAY(x, z+1, ARRAY_VERTEX+1);
        vert2[2] = z+1;

        vert3[0] = x+1;
        vert3[1] = TERRAIN_ARRAY(x+1, z+1, ARRAY_VERTEX+1);
        vert3[2] = z+1;
              
        if(intersect_triangle(orig, ray, vert0, vert2, vert1, &r1, &r2, &r3)) {
          if((r1 < minDist) && (r1 > 0)) {
            minDist = r1;
            minAngle = tempAngle;
          }
        }
        if(intersect_triangle(orig, ray, vert2, vert3, vert1, &r1, &r2, &r3)) {
          if((r1 < minDist) && (r1 > 0)) {
            minDist = r1;
            minAngle = tempAngle;
          }
        }
      }
    }
  }

  if(minDist == 100) return false;

  // Calculate the impact location
  tempAngleRadians = degreesToRadians((double) minAngle);
  minDX = sin(tempAngleRadians) * minDist;
  minDZ = cos(tempAngleRadians) * minDist;

  // Update the point to be the point of impact  
  p->x += minDX;
  p->y = 0;
  p->z += minDZ;

  *angle = minAngle;
    
  return true;
}
