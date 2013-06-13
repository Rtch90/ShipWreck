/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _SHIPAI_H_
#define _SHIPAI_H_

class Ship;

typedef enum {asFight, asFlee, asChase} AIState;

class ShipAI {
public:
  ShipAI(Ship *shipIn, Ship *playerShipIn);
  ~ShipAI();

  void update(unsigned int ticks, bool reloadedIn);

  bool shouldFire(float *angleOut);

private:
  Ship *ship;
  Ship *playerShip;

  AIState state;

  // Just so we don't have to de-reference a pointer over and over
  int shipMaxDamage;

  float rotToPlayer;
  float distToPlayer;

  bool reloaded;

  bool fire;
  float fireAngle;

  void updateFight(unsigned int ticks);
  void updateFlee(unsigned int ticks);
  void updateChase(unsigned int ticks);
};

#endif