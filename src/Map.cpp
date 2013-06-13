/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdlib>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Log.h"
#include "Config.h"
#include "image.h"
#include "font.h"
#include "GameState.h"
#include "StateSailing.h"
#include "Player.h"
#include "Map.h"
#include "CityManager.h"

Map::Map(StateSailing *curGame, Player *playerPos) : GameState(curGame) {

  winWidth = Config::configuration->getWinWidth();
  winHeight = Config::configuration->getWinHeight();

  // Set the pointer to the main sailing state to null
  mainGame = NULL;

  PlayerMap = playerPos;

  // Load the background
  glEnable(GL_TEXTURE_2D);
  BuildTexture("./data/mapscreen.png", &map_background, GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, map_background);

  curMousePosX  = 0;
  curMousePosY  = 0;

  info = false;

  transport = false;
}

Map::~Map() {
  glDeleteTextures(1, &map_background);
}

void Map::loop() {
  // Draw the scene
  display();

  // Process incoming events
  processEvents();
}

void Map::display() {

  float screenX, screenY;
  float multi;
  StateSailing* tempState;

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  // Draw the background
  // Bind the background texture
  glBindTexture(GL_TEXTURE_2D, map_background);

  // Set the color
  glColor3f(.6, .6, .6);

  // Draw the actual background quad
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0, 0);
  glTexCoord2f(0, 1);
  glVertex3f(0, winHeight, 0);
  glTexCoord2f(1, 1);
  glVertex3f(winWidth, winHeight, 0);
  glTexCoord2f(1, 0);
  glVertex3f(winWidth, 0, 0);
  glEnd();

  // Need to display the ship somehow!
  multi = winWidth / 812.0;
  screenX = (winWidth / 2) + (PlayerMap->x * multi);
  multi = winHeight / 500.0;
  screenY = (winHeight / 2) + (PlayerMap->z * multi);

  glColor3f(0, 1, 1); // This is the color of the X 

  glLineWidth(3.0);
  glBegin(GL_LINES);
  glVertex3f(screenX - 5.0, screenY + 5.0, 0);
  glVertex3f(screenX + 5.0, screenY - 5.0, 0);
  glVertex3f(screenX - 5.0, screenY - 5.0, 0);
  glVertex3f(screenX + 5.0, screenY + 5.0, 0);
  glEnd();
  glLineWidth(1.0);

  // Draw the text -- This is for debugging!
  if(info) {
    glTranslatef(0, (float) winHeight * .4, 0);
    glColor3f(0, 1, 1);

    glPrintf(0, "MapScreen x=%f z=%f", PlayerMap->x, PlayerMap->z);
    glNewLine();
    glPrintf(0, "OnMap x=%f y=%f", screenX, screenY);
    glNewLine();
  }

  glColor3f(1, 1, 1);

  tempState = (StateSailing*)previous;
    
  tempState->cities->MapDisplay();
  
  SDL_GL_SwapBuffers();
} 

void Map::processEvents() {
  // Our SDL event placeholder.
  SDL_Event event;
  
  handle_mouse_move();
  // Grab all the events off the queue.
  while( SDL_PollEvent( &event ) ) {
    switch( event.type ) {
    case SDL_KEYDOWN:
      //Handle key presses.
      handle_key_down( &event.key.keysym );
      break;
    case SDL_KEYUP:
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      //Handle quit requests (like Ctrl-c).
      doExit();
      break;
    }  
  }
}

void Map::handle_mouse_move() {
  float multi;
  Uint8 buttons;

  buttons = SDL_GetMouseState(&curMousePosX, &curMousePosY);

  if((buttons == SDL_BUTTON(1)) && transport) {
    multi = winWidth / 812.0;
    PlayerMap->x = -1*((winWidth / 2) - curMousePosX) / multi;
    multi = winHeight / 500.0;
    PlayerMap->z = -1*((winHeight / 2) - curMousePosY) / multi;
  }
}

void Map::handle_key_up(SDL_keysym* keysym) {
  switch(keysym->sym) {
  case SDLK_ESCAPE:
    // Go back to the game, if it is loaded
    if(previous != NULL) {
      switchState(previous, this);
    }
    break;
  case SDLK_m:
    // Go back to the game, if it is loaded
    if(previous != NULL) {
      switchState(previous, this);
    }
    break;
  case SDLK_t:
    // Stop the transport function
    transport = false;
    break;
  case SDLK_i:
    // Turn info on/off
    info = ((info) ? false : true);
    break;

  default:
    break;
  }
}

void Map::handle_key_down(SDL_keysym* keysym) {
  switch( keysym->sym ) {
  case SDLK_z:
    takeScreenshot();
    break;
  case SDLK_t:
    transport = true;
    break;
  default:
    break;
  }

}

void Map::doExit() {
  *done = true;
}

void Map::switchTo(GameState *oldState) {

  EnterTextMode(winWidth, winHeight); // What is this? Why do I need this here?

  transport = false;
}

void Map::switchFrom() {
}

void Map::drawGui() {
  glPushMatrix();
  glLoadIdentity();
  glBindTexture(GL_TEXTURE_2D, map_background);

  glColor3f(0, 0, 0);

  //Draw quad for map
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex3f(10, winHeight - 250, 0);
  glTexCoord2f(0, 1);
  glVertex3f(10, winHeight - 10, 0);
  glTexCoord2f(1, 1);
  glVertex3f(370, winHeight - 10, 0);
  glTexCoord2f(1, 0);
  glVertex3f(370, winHeight - 250, 0);
  glEnd();

  glColor3f(1, 1, 1);
  glPopMatrix();
}
