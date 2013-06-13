/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Log.h"
#include "image.h"
#include "BoundBox.h"
#include "extensions.h"
#include "Model.h"

// Helpers for loadObj
#define T(x) (model->triangles[(x)])

struct ObjTriangle {
  GLuint vindices[3];           // Array of triangle vertex indices
  GLuint nindices[3];           // Array of triangle normal indices
  GLuint tindices[3];           // Array of triangle texcoord indices
  GLuint findex;                // index of triangle facet normal
};

struct ObjGroup {
  char*             name;          // Name of this group
  GLuint            numtriangles;  // Number of triangles in this group
  GLuint*           triangles;     // Array of triangle indices
  //GLuint            material;    // Index to material for group
  struct ObjGroup* next;           // Pointer to next group in model
};

/*
struct ObjMaterial
{
  char* name;                   // Name of material
  GLfloat diffuse[4];           // Diffuse component
  GLfloat ambient[4];           // Ambient component
  GLfloat specular[4];          // Specular component
  GLfloat emmissive[4];         // Emmissive component
  GLfloat shininess;            // Specular exponent
};
*/

struct ObjModel {
  char*    pathname;            // Path to this model
  char*    mtllibname;          // Name of the material library

  GLuint   numvertices;         // Number of vertices in model
  GLfloat* vertices;            // Srray of vertices

  GLuint   numnormals;          // Number of normals in model
  GLfloat* normals;             // Array of normals

  GLuint   numtexcoords;        // Number of texcoords in model
  GLfloat* texcoords;           // Array of texture coordinates

  GLuint   numfacetnorms;       // Number of facetnorms in model
  GLfloat* facetnorms;          // Array of facetnorms

  GLuint       numtriangles;    // Number of triangles in model
  ObjTriangle* triangles;       // Array of triangles */

  //GLuint       nummaterials;    // Number of materials in model
  //ObjMaterial* materials;       // Array of materials

  GLuint       numgroups;       // Number of groups in model
  ObjGroup*    groups;          // Linked list of groups

  GLfloat position[3];          // Position of the model

};

Model::Model(const char *textureName) {
  loaded = false;

  BuildTexture(textureName, &texture, GL_CLAMP_TO_EDGE, true);
  glBindTexture(GL_TEXTURE_2D, texture);
}

Model::~Model() {
  glDeleteTextures(1, &texture);

  glDeleteLists(displayList, 1);

  delete modelBox;
}

void Model::draw() {
  if (loaded) {
    glCallList(displayList);
  }
}

void Model::startList() {
  GLfloat material[4];

  material[0] = 1.0;
  material[1] = 1.0;
  material[2] = 1.0;
  material[3] = 1.0;

  displayList = glGenLists(1);
  if (displayList == 0) {
    // Probably should throw and exception
    Log::logger->message("Could not generate model display list");
    return;
  }

  glNewList(displayList, GL_COMPILE);

  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

  glBindTexture(GL_TEXTURE_2D, texture);
}

void Model::loadObj(const char *filename, float scale) {
  ObjModel *model;

  model = objLoad(filename);
  if (!model) {
    return;
  }

  // Scale it properly
  objScale(model, scale);

  modelBox = objGenerateBox(model);

  startList();
  objDraw(model);
  glEndList();

  loaded = true;
}

ObjModel* Model::objLoad(const char *filename) {
  ObjModel* model;
  FILE*   file;

  /* open the file */
  file = fopen(filename, "r");
  if (!file) {
    // Probably should throw an exception
    Log::logger->message("Warning: Failed to open file %s", filename);
    return NULL;
  }

  /* allocate a new model */
  model = (ObjModel*)malloc(sizeof(ObjModel));
  model->pathname    = strdup(filename);
  model->mtllibname    = NULL;
  model->numvertices   = 0;
  model->vertices    = NULL;
  model->numnormals    = 0;
  model->normals     = NULL;
  model->numtexcoords  = 0;
  model->texcoords       = NULL;
  model->numfacetnorms = 0;
  model->facetnorms    = NULL;
  model->numtriangles  = 0;
  model->triangles       = NULL;
  //model->nummaterials  = 0;
  //model->materials       = NULL;
  model->numgroups       = 0;
  model->groups      = NULL;
  model->position[0]   = 0.0;
  model->position[1]   = 0.0;
  model->position[2]   = 0.0;

  /* Make a first pass through the file to get a count of the number
   * of vertices, normals, texcoords & triangles
   */
  objFirstPass(model, file);

  /* allocate memory */
  model->vertices = (GLfloat*)malloc(sizeof(GLfloat) *
                                     3 * (model->numvertices + 1));
  model->triangles = (ObjTriangle*)malloc(sizeof(ObjTriangle) *
                                          model->numtriangles);
  if (model->numnormals) {
    model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
                                      3 * (model->numnormals + 1));
  }
  if (model->numtexcoords) {
    model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *
                                        2 * (model->numtexcoords + 1));
  }

  // rewind to beginning of file and read in the data this pass
  rewind(file);

  objSecondPass(model, file);

  /* close the file */
  fclose(file);

  return model;
}

void Model::objDraw(ObjModel *model) {
  ObjGroup* group;
  ObjTriangle* triangle;

  glBegin(GL_TRIANGLES);

  group = model->groups;
  while (group) {
    for (unsigned int i = 0; i < group->numtriangles; i++) {
      triangle = &T(group->triangles[i]);

      glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
      glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
      glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);

      glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
      glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
      glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);

      glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
      glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
      glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
    }

    group = group->next;
  }

  glEnd();
}

void Model::objFirstPass(ObjModel *model, FILE* file) {
  GLuint  numvertices;        // Number of vertices in model
  GLuint  numnormals;         // Number of normals in model
  GLuint  numtexcoords;       // Number of texcoords in model
  GLuint  numtriangles;       // Number of triangles in model
  ObjGroup* group;            // Current group */
  unsigned    v, n, t;
  char        buf[128];

  /* Make a default group */
  group = objAddGroup(model, "default");

  numvertices = numnormals = numtexcoords = numtriangles = 0;
  while(fscanf(file, "%s", buf) != EOF) {
    switch(buf[0]) {
    case '#':               /* comment */
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    case 'v':               /* v, vn, vt */
      switch(buf[1]) {
      case '\0':          /* vertex */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        numvertices++;
        break;
      case 'n':           /* normal */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        numnormals++;
        break;
      case 't':           /* texcoord */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        numtexcoords++;
        break;
      default:
        printf("glmFirstPass(): Unknown token \"%s\".\n", buf);
        exit(1);
        break;
      }
      break;
    case 'm':
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    case 'u':
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    case 'g':               /* group */
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
      sscanf(buf, "%s", buf);
#else
      buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
      group = objAddGroup(model, buf);
      break;
    case 'f':               /* face */
      v = n = t = 0;
      fscanf(file, "%s", buf);
      if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
        /* v/t/n */
        fscanf(file, "%d/%d/%d", &v, &t, &n);
        fscanf(file, "%d/%d/%d", &v, &t, &n);
        numtriangles++;
        group->numtriangles++;
        while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
          numtriangles++;
          group->numtriangles++;
        }
      } else {
        Log::logger->message("Warning: Invalid .obj file");
      }
      break;

    default:
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    }
  }

  // Set the stats in the model structure
  model->numvertices  = numvertices;
  model->numnormals   = numnormals;
  model->numtexcoords = numtexcoords;
  model->numtriangles = numtriangles;

  // Allocate memory for the triangles in each group
  group = model->groups;
  while(group) {
    group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
    group->numtriangles = 0;
    group = group->next;
  }
}

ObjGroup* Model::objAddGroup(ObjModel *model, char *name) {
  ObjGroup* group;

  group = objFindGroup(model, name);
  if (!group) {
    group = (ObjGroup*)malloc(sizeof(ObjGroup));
    group->name = strdup(name);
    //group->material = 0;
    group->numtriangles = 0;
    group->triangles = NULL;
    group->next = model->groups;
    model->groups = group;
    model->numgroups++;
  }

  return group;
}

ObjGroup* Model::objFindGroup(ObjModel* model, char* name) {
  ObjGroup* group;

  group = model->groups;
  while(group) {
    if (!strcmp(name, group->name))
      break;
    group = group->next;
  }

  return group;
}

void Model::objSecondPass(ObjModel* model, FILE* file) {
  GLuint      numvertices;        // Number of vertices in model
  GLuint      numnormals;         // Number of normals in model
  GLuint      numtexcoords;       // Number of texcoords in model
  GLuint      numtriangles;       // Number of triangles in model
  GLfloat*    vertices;           // Array of vertices
  GLfloat*    normals;            // Array of normals
  GLfloat*    texcoords;          // Array of texture coordinates
  ObjGroup*   group;              // Current group pointer
  //GLuint  material;           // Current material
  GLuint  v, n, t;
  char        buf[128];

  /* set the pointer shortcuts */
  vertices       = model->vertices;
  normals    = model->normals;
  texcoords    = model->texcoords;
  group      = model->groups;

  /* On the second pass through the file, read all the data into the
   * allocated arrays
   */
  numvertices = numnormals = numtexcoords = 1;
  numtriangles = 0;
  //material = 0;
  while(fscanf(file, "%s", buf) != EOF) {
    switch(buf[0]) {
    case '#':               /* comment */
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    case 'v':               /* v, vn, vt */
      switch(buf[1]) {
      case '\0':          /* vertex */
        fscanf(file, "%f %f %f",
               &vertices[3 * numvertices + 0],
               &vertices[3 * numvertices + 1],
               &vertices[3 * numvertices + 2]);
        numvertices++;
        break;
      case 'n':           /* normal */
        fscanf(file, "%f %f %f",
               &normals[3 * numnormals + 0],
               &normals[3 * numnormals + 1],
               &normals[3 * numnormals + 2]);
        numnormals++;
        break;
      case 't':           /* texcoord */
        fscanf(file, "%f %f",
               &texcoords[2 * numtexcoords + 0],
               &texcoords[2 * numtexcoords + 1]);
        numtexcoords++;
        break;
      }
      break;
    case 'u':
      fgets(buf, sizeof(buf), file);
      sscanf(buf, "%s %s", buf, buf);
      //group->material = material = glmFindMaterial(model, buf);
      break;
    case 'g':               /* group */
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
      sscanf(buf, "%s", buf);
#else
      buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
      group = objFindGroup(model, buf);
      //group->material = material;
      break;
    case 'f':               /* face */
      v = n = t = 0;
      fscanf(file, "%s", buf);
      if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
        /* v/t/n */
        T(numtriangles).vindices[0] = v;
        T(numtriangles).tindices[0] = t;
        T(numtriangles).nindices[0] = n;
        fscanf(file, "%d/%d/%d", &v, &t, &n);
        T(numtriangles).vindices[1] = v;
        T(numtriangles).tindices[1] = t;
        T(numtriangles).nindices[1] = n;
        fscanf(file, "%d/%d/%d", &v, &t, &n);
        T(numtriangles).vindices[2] = v;
        T(numtriangles).tindices[2] = t;
        T(numtriangles).nindices[2] = n;
        group->triangles[group->numtriangles++] = numtriangles;
        numtriangles++;
        while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
          T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
          T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
          T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
          T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
          T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
          T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
          T(numtriangles).vindices[2] = v;
          T(numtriangles).tindices[2] = t;
          T(numtriangles).nindices[2] = n;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
        }
      } else {
        Log::logger->message("Warning: Invalid .obj file");
      }
      break;

    default:
      /* eat up rest of line */
      fgets(buf, sizeof(buf), file);
      break;
    }
  }
}

void Model::objScale(ObjModel* model, float scale) {
  GLuint i;

  for (i = 1; i <= model->numvertices; i++) {
    model->vertices[3 * i + 0] *= scale;
    model->vertices[3 * i + 1] *= scale;
    model->vertices[3 * i + 2] *= scale;
  }
}

BoundBox* Model::objGenerateBox(ObjModel* model) {
  float minX, maxX, minY, maxY, minZ, maxZ;
  float tempVertex;

  minX =  10000;
  maxX = -10000;
  minY =  10000;
  maxY = -10000;
  minZ =  10000;
  maxZ = -10000;

  for(unsigned int i = 1; i <= model->numvertices; i++) {
    tempVertex = model->vertices[3 * i + 0];
    if(tempVertex < minX) minX = tempVertex;
    if(tempVertex > maxX) maxX = tempVertex;

    tempVertex = model->vertices[3 * i + 1];
    if(tempVertex < minY) minY = tempVertex;
    if(tempVertex > maxY) maxY = tempVertex;

    tempVertex = model->vertices[3 * i + 2];
    if(tempVertex < minZ) minZ = tempVertex;
    if(tempVertex > maxZ) maxZ = tempVertex;
  }

  return new BoundBox(minX, maxX, minY, maxY, minZ, maxZ);
}
