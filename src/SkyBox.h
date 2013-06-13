/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

class SkyBox {
public:
  // Constructor
  SkyBox(char const *texturedir);

  // Destructor
  ~SkyBox();

  // Draw the skybox
  void draw(float x,float y,float z);

private:
  // Texture handle
  GLuint textures[6];
};

#endif
