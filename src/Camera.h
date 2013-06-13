/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

typedef float vec3[3];

class Camera {
public:
  // Contructor, pass in window width & height
  Camera(float winRatio, float x, float y, float z);

  // Set the camera location
  void setPosition(float x, float y, float z);
  void setPosCalcAngle(float x,float y,float z);

  void changePosition(float dx,float dy,float dz);

  void getPosition(vec3& ret);

  // Update the ratio
  void changeRatio(float winRatio);

  // Set the projection matrix
  void setProjection();

  void setCamera();

  void setAngle(float a1,float a2);
  void changeAngle(float da1,float da2);

private:
  // Calculate the dist, angles based on position
  void calcValues();

  float ratio;
  float angle;
  float angle2;
  vec3 position;
};

#endif
