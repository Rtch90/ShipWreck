/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _MODEL_H_
#define _MODEL_H_


struct ObjModel;
struct ObjGroup;
class  BoundBox;

class Model {
public:
  // This just loads a texture, need to call a loadFORMAT metod
  Model(const char *textureName);

  // do whatever cleanup is necessary
  ~Model();

  // Draws the actual model
  void draw();

  // Add all file format dependant loadFORMAT functions here
  // Loads a .obj file
  void loadObj(const char *filename, float scale);

  BoundBox *modelBox;

private:
  // Starts and sets up display list, call from all LoadFORMAT functions
  void startList();

  // texture the model
  GLuint texture;

  // Display list to display the model
  // Eventually this will be replaced by a VBO/VA
  GLuint displayList;

  // Whether the model has been loaded or not
  bool loaded;

  // Add all loadFORMAT helper functions here
  ObjModel* objLoad(const char *filename);
  void objDraw(ObjModel *model);
  void objFirstPass(ObjModel *model, FILE *file);
  ObjGroup* objAddGroup(ObjModel *model, char *name);
  ObjGroup* objFindGroup(ObjModel *model, char *name);
  void objSecondPass(ObjModel* model, FILE* file);
  void objScale(ObjModel* model, float scale);
  BoundBox* objGenerateBox(ObjModel* model);
};

#endif 
