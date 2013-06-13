/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../ccmath.h"
#include "../Ships.h"
#include "Bullet.h"

#define BULLETSPEED   .006
#define BULLETGRAVITY -.000002

GLuint Bullet::bulletList = 0;

Bullet::Bullet(Ship *ship, float angleIn) {
  float shipX = cos(-1*ship->rot) * ship->speed;
  float shipZ = sin(-1*ship->rot) * ship->speed;

  owner = ship;

  location[0] = owner->x;
  location[1] = .01;
  location[2] = owner->z;

  vector[0] = cos(angleIn) * BULLETSPEED + shipX;
  vector[1] = .0013;
  vector[2] = sin(angleIn) * BULLETSPEED + shipZ;

  // This realy should be ship->rot, not angleIn
  angle = radiansToDegrees(ship->rot);
}

Bullet::~Bullet() {
}

void Bullet::update(unsigned int ticks) {
  location[0] += vector[0] * ticks;
  location[1] += vector[1] * ticks;
  location[2] += vector[2] * ticks;

  vector[1] += BULLETGRAVITY * ticks;
}

bool Bullet::checkCollision(Ship *ship) {
  if((location[0]-ship->x)*(location[0]-ship->x) + (location[2]-ship->z)*(location[2]-ship->z) <= 1) {
    return true;
  } else {
    return false;
  }
}

void Bullet::draw() {
  glPushMatrix();
  glTranslatef(location[0], location[1], location[2]);
  glRotatef(angle, 0, 1, 0);
  glCallList(bulletList);
  glPopMatrix();
}

void Bullet::generateList() {
  // Generate the bullet list
  bulletList = glGenLists(1);
  if(bulletList == 0) {
    // Should throw an exception
    return;
  }
  
  glNewList(bulletList, GL_COMPILE);
  // Draw bullet here
  
  glPushMatrix();
  drawBullet();
  glTranslatef(0.3, 0, 0);
  drawBullet();
  glTranslatef(-0.6, 0, 0);
  drawBullet();
  glPopMatrix();
  glEndList();
}

void Bullet::deleteList() {
  glDeleteLists(bulletList, 1);
}
 
void Bullet::drawBullet() {
  glBegin(GL_TRIANGLES);
  glNormal3f(1, 0, 0);
  glVertex3f(.1, 0, 0);
  glNormal3f(0, 1, 0);
  glVertex3f(0, .1, 0);
  glNormal3f(0, 0, 1);
  glVertex3f(0, 0, .1);
  
  glNormal3f(0, 1, 0);
  glVertex3f(0, .1, 0);
  glNormal3f(-1, 0, 0);
  glVertex3f(-.1, 0, 0);
  glNormal3f(0, 0, 1);
  glVertex3f(0, 0, .1);
  
  glNormal3f(0, 1, 0);
  glVertex3f(0, .1, 0);
  glNormal3f(0, 0, -1);
  glVertex3f(0, 0, -.1);
  glNormal3f(-1, 0, 0);
  glVertex3f(-.1, 0, 0);
  
  glNormal3f(0, 1, 0);
  glVertex3f(0, .1, 0);
  glNormal3f(1, 0, 0);
  glVertex3f(.1, 0, 0);
  glNormal3f(0, 0, -1);
  glVertex3f(0, 0, -.1);

  glNormal3f(1, 0, 0);
  glVertex3f(.1, 0, 0);
  glNormal3f(0, 0, 1);
  glVertex3f(0, 0, .1);
  glNormal3f(0, -1, 0);
  glVertex3f(0, -.1, 0);
  
  glNormal3f(0, -1, 0);
  glVertex3f(0, -.1, 0);
  glNormal3f(0, 0, 1);
  glVertex3f(0, 0, .1);
  glNormal3f(-1, 0, 0);
  glVertex3f(-.1, 0, 0);
  
  glNormal3f(0, -1, 0);
  glVertex3f(0, -.1, 0);
  glNormal3f(-1, 0, 0);
  glVertex3f(-.1, 0, 0);
  glNormal3f(0, 0, -1);
  glVertex3f(0, 0, -.1);
  
  glNormal3f(0, -1, 0);
  glVertex3f(0, -.1, 0);
  glNormal3f(0, 0, -1);
  glVertex3f(0, 0, -.1);
  glNormal3f(1, 0, 0);
  glVertex3f(.1, 0, 0);
  glEnd();
}
