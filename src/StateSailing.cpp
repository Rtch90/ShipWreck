/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdlib.h>
#include <string>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "ccmath.h"
#include "Log.h"
#include "Config.h"
#include "normals.h"
#include "Player.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "Ocean.h"
#include "Camera.h"
#include "image.h"
#include "font.h"
#include "GameState.h"
#include "Map.h"
#include "City.h"
#include "CityManager.h"
#include "StateCity.h"
#include "Economy.h"
#include "Ships.h"
#include "StateBattle.h"
#include "StateSailing.h"

using namespace std;

#define WATER_SIZE 2.5

#define TICKS_BETWEEN_BATTLES 15000

StateSailing::StateSailing() : GameState(NULL) {
  bool EconCreated;

  // Temp storage for window width and height
  int width  = Config::configuration->getWinWidth();
  int height = Config::configuration->getWinHeight();

  displaySplashScreen("./data/loading.png", "Loading...", .4);

  Log::logger->message("Loading screen displayed");

  // Set the childState pointer to NULL
  childState = NULL;

  setTicksToNextBattle();

  // Initialize variables for left & right arrows
  keys[0] = false;
  keys[1] = false;

  // Initialize sway of the ship
  rotx = 0.0;

  // Create the ship list
  // This should only happen once here
  ShipManager::initShipList("./data/Ships.xml");

  // Set up the camera
  cameraX = 9;
  cameraY = 5;
  cameraZ = 0;
  camera = new Camera((float) width/(float) height, cameraX, cameraY, cameraZ);

  Log::logger->message("Camera created");

  /*
   * Create the Economy
   * This has been moved to forward in the order of creating
   * things because other things (ie. player) need it.
   * It does not depend on anything else in it's creation.
   */
  EconCreated = Economy::Econ->CreateEcon();

  if(EconCreated) Log::logger->message("Economy loaded");
  else Log::logger->message("***Economy loading failed***");

  // Create the player data
  player = new Player();

  Log::logger->message("Player created");

  // Create the skybox
  skybox = new SkyBox("./data/skybox1");

  // Create the terrain
  land = new Terrain("./data/heightmap.png", .04, 25);

  Log::logger->message("Terrain created");

  // Create the map
  map = new Map(this, player);

  Log::logger->message("Map created");

  // Create a city please.
  cities = new CityManager(land);

  Log::logger->message("Cities loaded");

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

  // Begin setting up the rendering

  glClearColor(0,0,0,0);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  //glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);

  // Set up lighting
  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);

  Log::logger->message("StateSailing initialized\n");

  lastTicks = SDL_GetTicks();
}

StateSailing::~StateSailing() {
  // Needs to delete all data
  
  // Delete all the objects
  delete player;
  delete skybox;
  delete land;
  delete camera;
  delete map;
  //delete economy;

  // wtf? Delete the childstate if it is active
  if(childState != NULL) {
    delete childState;
    childState = NULL;
  }

  ShipManager::deleteShipList();
  
  Log::logger->message("StateSailing deleted");
}

void StateSailing::newGame() {
  player->reset();
}

void StateSailing::saveGame() {
  player->save(0);
}


void StateSailing::loadGame() {
  player->load(0);
}
void StateSailing::loop() {
  // Check for child class. If it is there it means we switched to it, and now back, so it can be deleted.
  if(childState != NULL) {
    delete childState;
    childState = NULL;
  }
  // Draw the scene
  display();

  // Process incoming events
  processEvents();

  // Update the scene
  timer();
}

void StateSailing::processEvents() {
  // Our SDL event placeholder.
  SDL_Event event;

  static unsigned midx = Config::configuration->getWinWidth()/2,midy = Config::configuration->getWinHeight()/2;

  // Grab all the events off the queue.
  while(SDL_PollEvent(&event)) {
    switch( event.type ) {
    case SDL_KEYDOWN:
     // Handle key presses.
      handle_key_down( &event.key.keysym );
      break;
    case SDL_KEYUP:
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      /* Handle quit requests (like Ctrl-c). */
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

void StateSailing::handle_key_up(SDL_keysym* keysym) {
  switch(keysym->sym) {
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
  case SDLK_m:
    switchState(map, this);
    break;
  default:
    break;
  }
}

void StateSailing::handle_key_down(SDL_keysym* keysym) {
  switch(keysym->sym) {
  case SDLK_b:
    setTicksToNextBattle();
    if(childState != NULL) {
      delete childState;
    }
    childState = new StateBattle(player);
    switchState(childState, this);
    break;
  case SDLK_c:
    player->rot += M_PI;
    break;
  case SDLK_x:
    player->dump();
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
    player->speed += .005;
    break;
  case SDLK_s:
  case SDLK_DOWN:
    player->speed -= .005;
  default:
    break;
  }
  if(player->speed < 0) player->speed = 0;
  if(player->speed > .02) player->speed = .02;
}

void StateSailing::timer() {
  float shipSine, shipCos;
  GLfloat dX, dZ;
  double shipLine[3];
  int maxX, maxZ;

  maxX = land->getWidth()  / 2 - 5;
  maxZ = land->getHeight() / 2 - 5;

  // Used to check for collision with a city
  City *town;

  unsigned int tTicks = SDL_GetTicks();
  unsigned int ticks  = abs((int) (tTicks - lastTicks));
  lastTicks = tTicks;

  if(keys[0]) player->rot += .001*ticks;
  if(keys[1]) player->rot -= .001*ticks;
  if(player->rot >= TWO_PI) player->rot -= TWO_PI;
  if(player->rot < 0) player->rot += TWO_PI;

  shipSine = sin(player->rot);
  shipCos = cos(player->rot);

  shipLine[0] = -shipCos;
  shipLine[1] = 0;
  shipLine[2] = shipSine;

  // check for hitting a city.
  town = cities->CheckCollision(player);
  if(town != NULL) {
    player->speed = 0;
    player->rot += M_PI;
    if(player->rot > TWO_PI) player->rot -= TWO_PI;

    if(childState != NULL) {
      Log::logger->message("Warning: childState not null when entering town");
      delete childState;
      childState = NULL;
    }
    childState = new StateCity(town, player);
    switchState(childState, this);
  }


  if((player->speed > 0) && (land->checkCollision_Line(player->x, player->z, 6, shipLine, 1.25))) {
    player->speed = 0;
  } else {
    dX = shipCos * player->speed * ticks;
    dZ = shipSine * player->speed * ticks;

    player->x -= dX;
    player->z += dZ;

    if(player->x > maxX) {
      player->x = maxX;
    } else if(player->x < -maxX) {
      player->x = -maxX;
    }
    if(player->z > maxZ) {
      player->z = maxZ;
    } else if(player->z < -maxZ) {
      player->z = -maxX;
    }
  }

  land->update(ticks);

  rotx += .001 * ticks;
  if(rotx > TWO_PI) rotx -= TWO_PI;

  ticksToNextBattle -= ticks;
  if(ticksToNextBattle <= 0) {
    setTicksToNextBattle();

    if(childState != NULL) {
      Log::logger->message("Warning: childstate not null when entering naval battle");
      delete childState;
      childState = NULL;
    }
    childState = new StateBattle(player);
    switchState(childState, this);
  }
}

void StateSailing::display() {
  // Don't need to clear color buffer, since every pixel is redrawn
  // Just do it anyway for now.
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  camera->setCamera();

  // Fuck, I was sat here wondering if this was needed, then it hit me, 'tis for skybox.
  glPushMatrix();
  glRotatef(-(radiansToDegrees(player->rot) + 180), 0, 1, 0);
  skybox->draw(cameraX, cameraY, cameraZ);
  glPopMatrix();

  //glDisable(GL_TEXTURE_2D);

  // Draw the ship
  glPushMatrix();
  glRotatef(5*sin(rotx), 1, 0, 0);
  player->ship->draw();
  glPopMatrix();

  //glEnable(GL_TEXTURE_2D);

  // Draw the terrain
  glPushMatrix();
  glRotatef(-(radiansToDegrees(player->rot) + 180), 0, 1, 0);
  glTranslatef(-1*player->x, 0, -1*player->z);
  land->draw();
  cities->DisplayCities();
  glPopMatrix();

  SDL_GL_SwapBuffers();
}

void StateSailing::setTicksToNextBattle() {
  ticksToNextBattle = (int) (TICKS_BETWEEN_BATTLES * (randBellCurve() + .5));
}

void StateSailing::switchTo(GameState *oldState) {
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

void StateSailing::switchFrom() {
}
