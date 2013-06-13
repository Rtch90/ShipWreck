/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <list>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../ccmath.h"
#include "../Log.h"
#include "../Config.h"
#include "../Player.h"
#include "../Camera.h"
#include "../image.h"
#include "../GameState.h"
#include "../Ships.h"
#include "Bullet.h"
#include "ShipAI.h"
#include "StateDone.h"
#include "Battle.h"

#define RELOADTICKS 1500

#define WATER_TEXSCALE 6
#define WATER_SIZE 200

using namespace std;

Battle::Battle(Player *playerIn): GameState(NULL) {
  int shipId;

  // Temp storage for window width and height
  int width  = Config::configuration->getWinWidth();
  int height = Config::configuration->getWinHeight();
  
  // Just to be safe
  leaveState = NULL;

  // Initialize variables for left and right arrows
  keys[0] = false;
  keys[1] = false;

  // Initialize sway of the ship
  rotx = 0.0;

  player = playerIn;

  // Get a copy of the player's ship
  ship = new Ship(player->ship);
  ship->x = 0;
  ship->z = 0;
  ship->rot = randInt(359);
  ship->speed = 0.002;
  //ship->speed = 0;
  playerTicksToReload = 0;

  // Set up the enemy ship
  shipId = randInt(3);
  enemyShip = ShipManager::list->newShip(shipId, "Enemy");
  enemyShip->x = randInt(5);
  enemyShip->z = randInt(5);
  if(((enemyShip->x*enemyShip->x)+(enemyShip->z*enemyShip->z)) < 4) {
    if(enemyShip->x > 0) {
      enemyShip->x += 2;
    } else {
      enemyShip->x -= 2;
    }
  }
  enemyShip->rot = randInt(359);
  enemyShip->speed = 0.002;
  //enemyShip->speed = 0;
  enemyTicksToReload = 0;

  enemyAI = new ShipAI(enemyShip, ship);

  Bullet::generateList();

  // Set up the camera
  cameraX = 9;
  cameraY = 5;
  cameraZ = 0;
  camera  = new Camera((float) width/(float) height, cameraX, cameraY, cameraZ); 

  // Set the light color and position
  ambientLight[0] = 0.17;
  ambientLight[1] = 0.17;
  ambientLight[2] = 0.17;
  ambientLight[3] = 1.0;

  diffuseLight[0] = 1.0;
  diffuseLight[1] = 1.0;
  diffuseLight[2] = 1.0;
  diffuseLight[3] = 1.0;

  lightPosition[0] = 7.0;
  lightPosition[1] = 15.0;
  lightPosition[2] = 2.0;
  lightPosition[3] = 0.0;

  waterMaterial[0] = 0.1;
  waterMaterial[1] = 0.15;
  waterMaterial[2] = 0.3;
  waterMaterial[3] = 1.0;

  bulletMaterial[0] = 0.1;
  bulletMaterial[1] = 0.1;
  bulletMaterial[2] = 0.1;
  bulletMaterial[3] = 1.0;
  
  BuildTexture("./data/water.png", &waterTexture, GL_REPEAT, true);
  
  // Begin setting up the rendering

  glClearColor(0, 0, 0, 0);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  //glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);

  glPointSize(5);

  // Set up lighting
  // For some reason this messes up the lighting in StateSailing
  //glEnable(GL_LIGHTING);
  //glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  //glEnable(GL_LIGHT0);
  
  lastTicks = SDL_GetTicks();
}

Battle::~Battle() {
  Bullet *temp;

  while(!bullets.empty()) {
    temp = bullets.front();
    bullets.pop_front();
    delete temp;
  }
  delete enemyAI;

  delete ship;
  delete enemyShip;
  delete camera;
  
  if(leaveState != NULL) {
    delete leaveState;
    leaveState = NULL;
  }

  Bullet::deleteList();
  
  Log::logger->message("Battle deleted");
}

void Battle::loop() {
  // Draw the scene
  display();

  // Process incoming events
  processEvents();

  // Update the scene
  timer();
}

void Battle::processEvents() {
  /* Our SDL event placeholder. */
  SDL_Event event;

  static unsigned midx = Config::configuration->getWinWidth()/2,midy = Config::configuration->getWinHeight()/2;
  
  // Grab all the events off the queue.
  while( SDL_PollEvent( &event ) ) {
    switch( event.type ) {
    case SDL_KEYDOWN:
      // Handle key presses.
      handle_key_down( &event.key.keysym );
      break;
    case SDL_KEYUP:
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      // Handle quit requests (like Ctrl-c).
      mainMenu->doExit();
      break;
    case SDL_MOUSEMOTION:
      if(event.motion.x != midx || event.motion.y != midy) {
	this->camera->changeAngle(0.2*event.motion.yrel,0.2*event.motion.xrel);
        SDL_WarpMouse(midx,midy);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      vec3 v;
      this->camera->getPosition(v);
      float mag = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
      v[0] /= mag;
      v[1] /= mag;
      v[2] /= mag;
      if(v[0] < 0)
        v[0] = -v[0];
      if(v[1] < 0)
        v[1] = -v[1];
      if(v[2] < 0)
        v[2] = -v[2];
      if(event.button.button == SDL_BUTTON_WHEELUP && mag >= 1)
        this->camera->changePosition(-v[0],-v[1],-v[2]);
      else if(event.button.button == SDL_BUTTON_WHEELDOWN)
        this->camera->changePosition(v[0],v[1],v[2]);
      else
        break;
      this->camera->getPosition(v);
      break;
    }  
  }
}

void Battle::handle_key_up(SDL_keysym* keysym) {
  switch( keysym->sym ) {
  case SDLK_a:
  case SDLK_LEFT:
    keys[0] = false;
    break;
  case SDLK_d:
  case SDLK_RIGHT:
    keys[1] = false;
    break;
  case SDLK_ESCAPE:
    switchState(mainMenu, this);
    break;
  default:
    break;
  }
}

void Battle::handle_key_down(SDL_keysym* keysym) {
  switch( keysym->sym ) {
  case SDLK_e:
    playerFire(-1*M_PI_2);
    break;
  case SDLK_q:
    playerFire(M_PI_2);
    break;
  case SDLK_o:
    enemyFire(-1*M_PI_2);
    break;
  case SDLK_p:
    enemyFire(M_PI_2);
    break;
  case SDLK_c:
    ship->rot += M_PI;
    break;
  case SDLK_x:
    battleFinished(sbNone);
    break;
  case SDLK_z:
    takeScreenshot();
    break;
  case SDLK_a:
  case SDLK_LEFT:
    keys[0] = true;
    break;
  case SDLK_d:
  case SDLK_RIGHT:
    keys[1] = true;
    break;
  case SDLK_w:
  case SDLK_UP:
    ship->speed += .002;
    break;
  case SDLK_s:
  case SDLK_DOWN:
    ship->speed -= .002;
  default:
    break;
  }
  if(ship->speed < 0) ship->speed = 0;
  if(ship->speed > .008) ship->speed = .008;
}

void Battle::timer() {
  float shipSine, shipCos;
  GLfloat dX, dZ;
  list< Bullet* >::iterator iter;
  list< Bullet* >::iterator iterEnd;
  Bullet *temp;
  Ship *tempShip;
  GLfloat distBetweenShips;
  float tempAngle;

  unsigned int tTicks = SDL_GetTicks();
  unsigned int ticks = abs((int) (tTicks - lastTicks));
  lastTicks = tTicks;

  if(playerTicksToReload > 0) {
    playerTicksToReload -= ticks;
  }
  if(enemyTicksToReload > 0) {
    enemyTicksToReload -= ticks;
  }

  enemyAI->update(ticks, enemyTicksToReload <= 0);
  if(enemyAI->shouldFire(&tempAngle)) {
    enemyFire(tempAngle);
  }
  
  if(keys[0]) ship->rot += .001*ticks;
  if(keys[1]) ship->rot -= .001*ticks;
  if(ship->rot >= TWO_PI) ship->rot -= TWO_PI;
  if(ship->rot < 0) ship->rot += TWO_PI;

  shipSine = sin(ship->rot);
  shipCos  = cos(ship->rot);
  dX = shipCos  * ship->speed * ticks;
  dZ = shipSine * ship->speed * ticks;
  ship->x += dX;
  ship->z -= dZ;

  shipSine = sin(enemyShip->rot);
  shipCos  = cos(enemyShip->rot);
  dX = shipCos  * enemyShip->speed * ticks;
  dZ = shipSine * enemyShip->speed * ticks;
  enemyShip->x += dX;
  enemyShip->z -= dZ;  

  rotx += .001 * ticks;
  if(rotx > TWO_PI) rotx -= TWO_PI;

  temp = bullets.front();
  while(!bullets.empty() && temp->location[1] < -.01) {
    bullets.pop_front();
    delete temp;
    temp = bullets.front();
  }

  iter = bullets.begin();
  iterEnd = bullets.end();
  while(iter != iterEnd) {
    temp = *iter;
    
    if(temp->location[1] >= -.01) {
      temp->update(ticks);
    
      if(temp->owner == ship) {
        tempShip = enemyShip;
      } else {
        tempShip = ship;
      }
    
      if(temp->checkCollision(tempShip)) {
        temp->location[1] = -10;
        --tempShip->damage;        
        
        Log::logger->message("Damage: %d (%f)", tempShip->damage, temp->location[1]);
      }
    }
    
    ++iter;
  }
  
  if(ship->damage <= 0) {
    battleFinished(sbDefeat);
  } else if(enemyShip->damage <= 0) {
    battleFinished(sbVictory);
  }
  
  distBetweenShips = (ship->x - enemyShip->x)*(ship->x - enemyShip->x)+(ship->z - enemyShip->z)*(ship->z - enemyShip->z);
  if(distBetweenShips > 500) {
    battleFinished(sbNone);
  }
}

void Battle::display() {
  float texShiftX = ship->x / WATER_TEXSCALE;
  float texShiftZ = ship->z / WATER_TEXSCALE;
  Bullet *temp;
  list< Bullet* >::iterator iter;
  list< Bullet* >::iterator iterEnd;

  // Don't need to clear color buffer, since every pixel is redrawn
  // Just do it anyway, while it's in development
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  camera->setCamera();

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, waterMaterial);
  glBindTexture(GL_TEXTURE_2D, waterTexture);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0+texShiftX, 0+texShiftZ);
  glVertex3f(-WATER_SIZE/2, 0, -WATER_SIZE/2);
  glTexCoord2f(0+texShiftX, WATER_SIZE/WATER_TEXSCALE+texShiftZ);
  glVertex3f(-WATER_SIZE/2, 0, WATER_SIZE/2);
  glTexCoord2f(WATER_SIZE/WATER_TEXSCALE+texShiftX, WATER_SIZE/WATER_TEXSCALE+texShiftZ);
  glVertex3f(WATER_SIZE/2, 0, WATER_SIZE/2);
  glTexCoord2f(WATER_SIZE/WATER_TEXSCALE+texShiftX, 0+texShiftZ);
  glVertex3f(WATER_SIZE/2, 0, -WATER_SIZE/2);
  glEnd();

  // Draw the ship
  glPushMatrix();
  glRotatef(radiansToDegrees(ship->rot), 0, 1, 0);
  glRotatef(5*sin(rotx), 1, 0, 0);
  ship->draw();
  glPopMatrix();

  // Draw the enemy ship
  glPushMatrix();
  glTranslatef(enemyShip->x-ship->x, 0, enemyShip->z-ship->z);
  glRotatef(radiansToDegrees(enemyShip->rot), 0, 1, 0);
  glRotatef(5*sin(rotx), 1, 0, 0);
  enemyShip->draw();
  glPopMatrix();

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bulletMaterial);

  glPushMatrix();
  glTranslatef(-1*ship->x, 0, -1*ship->z);
  glDisable(GL_TEXTURE_2D);

  iter = bullets.begin();
  iterEnd = bullets.end();
  while(iter != iterEnd) {
    temp = *iter;
    temp->draw();
    ++iter;
  }
  glEnable(GL_TEXTURE_2D);
  glPopMatrix();


  SDL_GL_SwapBuffers();
}

void Battle::playerFire(float rotDiff) {
  // Change this to make sure reloaded
  if(playerTicksToReload <= 0) {
    float angle = ship->rot + rotDiff;
    bullets.push_back(new Bullet(ship, -1*angle));
    playerTicksToReload = RELOADTICKS;
  }
}
void Battle::enemyFire(float rotDiff) {
  // Change this to make sure reloaded
  if(enemyTicksToReload <= 0) {
    float angle = enemyShip->rot + rotDiff;
    bullets.push_back(new Bullet(enemyShip, -1*angle));
    enemyTicksToReload = RELOADTICKS;
  }
}

void Battle::playerHit() {
  Log::logger->message("Player hit");
}

void Battle::enemyHit() {
  Log::logger->message("Enemy hit");
}

void Battle::battleFinished(BattleResult result) {
  // Just to be safe
  if(leaveState != NULL) {
    delete leaveState;
  }

  leaveState = new StateDone(player, enemyShip, result);
  switchState(leaveState, previous);
}

void Battle::switchTo(GameState *oldState) {
  keys[0] = false;
  keys[1] = false;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  camera->setProjection();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  
 lastTicks = SDL_GetTicks();
}

void Battle::switchFrom() {
}
