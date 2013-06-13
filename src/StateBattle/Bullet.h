/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _BULLET_H_
#define _BULLET_H_

class Ship;

class Bullet {
public:
  Bullet(Ship *ship, float angleIn);
  ~Bullet();

  void update(unsigned int ticks);

  bool checkCollision(Ship *ship);

  void draw();

  static void generateList();
  static void deleteList();

  float location[3];
  float vector[3];

  Ship *owner;

private:
  // Just used to draw the bullet (stored in degrees!)
  float angle;

  static GLuint bulletList;

  // Draws one cannon ball, used to generate list
  static void drawBullet();
};

#endif