/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include "Log.h"
#include "Config.h"
#include "image.h"
#include "font.h"
#include "GameState.h"

using namespace std;

GameState *GameState::mainMenu = NULL;
GameState **GameState::currentPtr = NULL;

unsigned char GameState::nextScreen = 0;

GameState::GameState(GameState *prev) {
  previous = prev;
}

// Destructor
// This has basicaly nothing to do.
GameState::~GameState() {
}

// Static function to set mainMenu pointer
void GameState::setMainMenu(GameState *menu) {
  if((menu != NULL) && (mainMenu != NULL)) {
    Log::logger->message("Warning: setMainMenu called with non-null mainMenu pointer");
  }
  mainMenu = menu;
}

// static function to set currentPtr pointer
void GameState::setCurrentPtr(GameState **current) {
  if(currentPtr != NULL) {
    Log::logger->message("Warning: setCurrentPtr called with non-null currentPtr pointer");
  }
  currentPtr = current;
}

void GameState::doExit() {
  // The MainMenu object needs to overload this
  if (mainMenu != NULL) {
    mainMenu->doExit();
  }
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
const Uint32 Rmask = 0xFF,
             Gmask = 0xFF<<8,
             Bmask = 0xFF<<(2*8),
             Amask = 0xFF<<(3*8);
#else
const Uint32 Rmask = 0xFF<<(3*8),
             Gmask = 0xFF<<(2*8),
             Bmask = 0xFF<<8,
             Amask = 0xFF;
#endif

void GameState::takeScreenshot() {
  //char buf[strlen("screenshotXXX.bmp")+1];   Need to look at this doesn't compile under vs for windows
  char buf[18];
  FILE* fp;

  unsigned count = nextScreen;

  //int width  = Config::configuration->getWinWidth();
  //int height = Config::configuration->getWinHeight();

  while(1) {
    if(count > 255) {
      Log::logger->message("Error: All 256 screenshot names are taken, could not take screenshot");
      return;
    }

#ifdef WIN32
    _snprintf(buf,strlen("screenshotXXX.bmp")+1,"Screenshot%u.bmp",count);
#endif
    snprintf(buf,strlen("screenshotXXX.bmp")+1,"Screenshot%u.bmp",count);
    
    // Check to see if the file already exists
    if(!(fp = fopen(buf,"r")))
      // The file does not exist, so may as well use it.
      break;
    fclose(fp);

    ++count;
  }

  nextScreen = count;

// Ginto is currently trying to make this work
// TODO: get openGL pixel data into a texture, then get pixel data from texture

  int w = Config::configuration->getWinWidth(),h = Config::configuration->getWinHeight();
  // get previous frame, because the current may not have been drawn yet
  glReadBuffer(GL_FRONT);

  Uint32* pixels = new Uint32[w*h];

  glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels);

  // flip image vertically because GL normally gives pixels upside down
  Uint32* tmpBuf = new Uint32[w];
  for(int y=0;y<h/2;++y) {
      memcpy(tmpBuf,pixels+y*w,w*sizeof(Uint32));
      memcpy(pixels+y*w,pixels+(h-y-1)*w,w*sizeof(Uint32));
      memcpy(pixels+(h-y-1)*w,tmpBuf,w*sizeof(Uint32));
  }
  delete tmpBuf;

  SDL_Surface* img = SDL_CreateRGBSurfaceFrom(pixels,w,h,32,w*4,Rmask,Gmask,Bmask,Amask);

  SDL_SaveBMP(img,buf);

  SDL_FreeSurface(img);
  delete pixels;
//  if(!(fp = fopen(buf,"w"))) {
//    Log::logger->message("could not open screenshot file for writing");
//    return;
//  }


}

void GameState::switchState(GameState *next, GameState *prev) {
  // Prepare to leave this state
  switchFrom();

  // Set the next state's previous pointer, if it's not NULL
  // If it is null, just leave the previous pointer alone
  if(prev != NULL) {
    next->previous = prev;
  }

  // Prepare to enter the next state
  next->switchTo(prev);

  // Hand control over to the next state
  *currentPtr = next;
}

void GameState::displaySplashScreen(string imageFile, string text, float shade) {
  GLuint loadingTexture;

  // Temp storage for window width and height
  int width  = Config::configuration->getWinWidth();
  int height = Config::configuration->getWinHeight();

  // Show the loading screen

  // Get ready for text mode
  EnterTextMode(width, height);
 
  glEnable(GL_TEXTURE_2D);
  BuildTexture(imageFile.c_str(), &loadingTexture, GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, loadingTexture);

  // Clear the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Set the color
  glColor3f(shade, shade, shade);

  // Draw the actual background quad
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0, 0);
  glTexCoord2f(0, 1);
  glVertex3f(0, height, 0);
  glTexCoord2f(1, 1);
  glVertex3f(width, height, 0);
  glTexCoord2f(1, 0);
  glVertex3f(width, 0, 0);
  glEnd();

  // Draw the text
  glTranslatef(0, (float) height * .5 - (16/2), 0);
  glTranslatef(width * 0.5 - (text.length() * 10 / 2), 0, 0);
  glColor3f(0, 0, 1);
  glPrint(text.c_str(), 0);

  SDL_GL_SwapBuffers();

  // Done with the texture so get shut of it
  glDeleteTextures(1, &loadingTexture);
}
