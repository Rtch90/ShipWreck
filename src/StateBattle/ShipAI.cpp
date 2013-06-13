/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include "../Log.h"
#include "../ccmath.h"
#include "../Ships.h"
#include "ShipAI.h"

#define CHASE_RANGE 150

ShipAI::ShipAI(Ship *shipIn, Ship *playerShipIn) {
  ship   = shipIn;
  playerShip = playerShipIn;

  state = asFight;

  reloaded = true;

  shipMaxDamage = ship->getMaxDamage();
}

ShipAI::~ShipAI() {
  // Don't delete the ships, the Battle object will delete them
}

void ShipAI::update(unsigned int ticks, bool reloadedIn) {
  float dX, dZ;

  reloaded = reloadedIn;

  // It's weird that the order of these is reversed, but it works anyhow...
  dX = ship->x - playerShip->x;
  dZ = playerShip->z - ship->z;

  distToPlayer = (dX*dX) + (dZ*dZ);

  if((state != asFlee) && (ship->damage < (shipMaxDamage / 2)) && (playerShip->damage > (ship->damage * 1.5))) {
    state = asFlee;
  }

  //rotToPlayer = atan2(dZ, dX) + M_PI;
  rotToPlayer = ship->rot - (atan2(dZ, dX) + M_PI);

  if(rotToPlayer < -M_PI) {
    rotToPlayer += TWO_PI;
  }
  if(rotToPlayer > M_PI) {
    rotToPlayer -= TWO_PI;
  }

  switch(state) {
    case asFight:
      updateFight(ticks);
      break;
    case asFlee:
      updateFlee(ticks);
      break;
    case asChase:
      updateChase(ticks);
      break;
  }
  if(ship->rot < 0) {
    ship->rot += TWO_PI;
  }
  if(ship->rot >= TWO_PI) {
    ship->rot  -= TWO_PI;
  }
  //Log::logger->message("%d %f %f %f", state, ship->rot, rotToPlayer, distToPlayer);
}

void ShipAI::updateFight(unsigned int ticks) {
  if(distToPlayer > CHASE_RANGE) {
    state = asChase;
  }
  
  if(reloaded) {
    if(abs(degreesToRadians(-90) - rotToPlayer) < degreesToRadians(10)) {
      fire = true;
      fireAngle = M_PI_2;
    } else if(abs(degreesToRadians(90) - rotToPlayer) < degreesToRadians(10)) {
      fire = true;
      fireAngle = -1*M_PI_2;
    }
  }

  if(((rotToPlayer < 0) && (rotToPlayer > degreesToRadians(-90))) || ((rotToPlayer > degreesToRadians(90)) && (rotToPlayer < degreesToRadians(180)))) {
    ship->rot -= .001*ticks;
  } else {
    ship->rot += .001*ticks;
  }
}

void ShipAI::updateFlee(unsigned int ticks) {
  if((rotToPlayer > 0) && (rotToPlayer < M_PI-0.03)) {
    ship->rot += .001 * ticks;
  } else if(rotToPlayer > 0.03-M_PI) {
    ship->rot -= .001 * ticks;
  }
}

void ShipAI::updateChase(unsigned int ticks) {
  if(distToPlayer < CHASE_RANGE-(CHASE_RANGE* .15)) {
    state = asFight;
  }

  if(rotToPlayer < -0.03) {
    ship->rot += .001 * ticks;
  } else if(rotToPlayer > 0.03) {
    ship->rot -= .001 *ticks;
  }
}

bool ShipAI::shouldFire(float *angleOut) {
  if(fire) {
    *angleOut = fireAngle;
    reloaded = false;
    return true;
  } else {
    return false;
  }
}
