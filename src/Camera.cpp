/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "ccmath.h"
#include "Camera.h"
#include <cstdio>

#if !defined (MSVC6)
using namespace std;
#endif

Camera::Camera(float winRatio, float x, float y, float z) {
  changeRatio(winRatio);

  // Set the camera position
  setPosCalcAngle(x, y, z);

  //calcValues();

  //setProjection();
}

void Camera::setPosition(float x, float y, float z) {
  position[0] = x;
  position[1] = y;
  position[2] = z;

  setCamera();
}

void Camera::setPosCalcAngle(float x, float y, float z) {
  position[0] = x;
  position[1] = y;
  position[2] = z;

  calcValues();
  setCamera();
}

void Camera::changeRatio(float winRatio) {
  ratio = winRatio;
}

void Camera::setProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, ratio, 1.0, 512.0);

  glMatrixMode(GL_MODELVIEW);
}

void Camera::calcValues() {
    angle = radiansToDegrees(atan2(position[1], abs(position[0])));
    angle2 = (180.0/M_PI)*atan2(position[0], position[2]);
}

void Camera::setCamera() {
  //OOOOOH, we DID set it for good... set it away wherever it was dropped, so randomly :P
  // This should stop it from bitching at us.
  setProjection();

  glRotatef(angle, 1.0, 0.0, 0.0);
  glRotatef(angle2, 0.0, 1.0, 0.0);
  glTranslatef(position[0], -1*position[1], -1*position[2]);
  return;
}

void Camera::setAngle(float a1,float a2) {
    angle = a1;
    angle2 = a2;
}

void Camera::changeAngle(float da1,float da2) {
    angle += da1;
    angle2 += da2;
}

void Camera::changePosition(float dx,float dy,float dz) {
    this->position[0] += dx;
    this->position[1] += dy;
    this->position[2] += dz;

    setCamera();
}

void Camera::getPosition(vec3 &ret) {
    ret[0] = this->position[0];
    ret[1] = this->position[1];
    ret[2] = this->position[2];
}
