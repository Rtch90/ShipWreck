/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdlib>
#include <string>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "AL/al.h"
#include "AL/alut.h"
#include "Log.h"
#include "Config.h"
#include "GameState.h"
#include "MainMenu.h"
#include "Economy.h"
#include "font.h"

using namespace std;

// Set to true to exit program
bool done = false;

GameState *current;
MainMenu  *mainMenu;

void prepExit() {
  SDL_Quit();
  Log::logger->message("SDL shutdown");
  Config::closeConfig();

  Log::logger->message("Shutdown sucessful");
  Log::closeLog();
}

void init() {
  int width  = Config::configuration->getWinWidth();
  int height = Config::configuration->getWinHeight();

  glViewport(0, 0, width, height);

  GameState::setCurrentPtr(&current);

  Log::logger->message("Current state pointer set");

  // Initialize the font
  if(!BuildFont("./data/font.png")) {
    // Error building font
    Log::logger->message("Error: Could not initialize font");
    prepExit();
    exit(1);
  }

  Log::logger->message("Fonts built");

  // Set up the mainMenu to run first
  mainMenu = new MainMenu(&done);
  current  = mainMenu;
  //mainMenu->setCurrentPtr(&current); //So the GameState can change where the current pointer points to, when changing state with switchState()
  Log::logger->message("Main menu created");
}

void shutdownAudio() {
  alutExit();
  alGetError();
  Log::logger->message("Audio shutdown");
}

void initAudio() {
  alutInit(0, NULL);

  Log::logger->message("Audio initialised");
}

int main(int argc, char** argv) {
  int width, height;

  Log::openLog(true);
  Config::openConfig("Config.xml");

  width  = Config::configuration->getWinWidth();
  height = Config::configuration->getWinHeight();

  //Information about the current video settings.
  const SDL_VideoInfo* info = NULL;

  // Color depth in bits of our window.
  int bpp = 0;
  // Flags we will pass into SDL_SetVideoMode.
  int flags = 0;

  // Flags for SDL_Init
  int initFlags = 0;
  
  if(Config::configuration->checkSDLParachute()) {
    initFlags = SDL_INIT_VIDEO;
  } else {
    initFlags = SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE;
  }

  // First, initialize SDL's video subsystem.
  if(SDL_Init(initFlags) < 0) {
    // Failed to initiate
    Log::logger->message("Video initialization failed: %s", SDL_GetError());

    prepExit();

    exit(1);
  }

  // Let us get some video information
  info = SDL_GetVideoInfo();
  if(!info) {
    // This should probably never happen.
    Log::logger->message("Video query failed: %s", SDL_GetError());

    prepExit();

    exit(1);
  }

  Log::logger->message("Got video info");

  /*
   * We get the bpp we will request from
   * the display. On X11, VidMode can't change
   * resolution, so this is probably being overly
   * safe. Under Win32, ChangeDisplaySettings
   * can change the bpp.
   */

  bpp = info->vfmt->BitsPerPixel;

  /*
   * Now, we want to setup our requested
   * window attributes for our OpenGL window.
   * We want _at least_ 5 bits of red, green
   * and blue. We also want at least a 16-bit
   * depth buffer.
   *
   * The last thing we do is request a double
   * buffered window. '1' turns on double
   * buffering, '0' turns it off.
   *
   * Note that we do not use SDL_DOUBLEBUF in
   * the flags to SDL_SetVideoMode. That does
   * not affect the GL attribute state, only
   * the standard 2D blitting setup.
   */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Request an OpenGL window
  if(Config::configuration->checkFullscreen()) {
    flags = SDL_OPENGL|SDL_FULLSCREEN;
  } else {
    flags = SDL_OPENGL;
  }

  // Set a window name.
  SDL_WM_SetCaption("Lephisto", "Lephisto");

   // Set the video mode

  if(SDL_SetVideoMode(width, height, bpp, flags) == 0) {
    /*
     * This could happen for a variety of reasons,
     * including DISPLAY not being set, the specified
     * resolution not being available, etc.
     */
    //fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
    Log::logger->message("Video mode set failed: %s", SDL_GetError());

    prepExit();

    exit(1);
  }


  Log::logger->message("Video mode set");

  // Make sure the video card suppors the necessary extensions, This needs to happen after we have valid openGL rendering contrext.
  if(!Config::configuration->checkExtensions()) {
    Log::logger->message("Error: Your Video card does not support the necessary extensions");
    prepExit();
    exit(1);
  }
  Log::logger->message("Extensions verified");

  initAudio();

  init();

  Log::logger->message("\nAnd Suddenly! Out of the depths of hell came LEPHISTO!");

  try {
    while(!done && (current != NULL)) {
      current->loop();
    }
  } catch (const char *e) {
    Log::logger->message("Uncaught exeption (%s)", e);
  } catch (string e) {
    Log::logger->message("Uncaught exeption (%s)", e.c_str());
  } catch (...) {
    Log::logger->message("Uncaught exeption");
  }

  delete mainMenu;
  
  // Don't delete current!
  // We only created the main menu, whatever created current will delete it
  //delete current;

  shutdownAudio();

  // Done with the fonts now
  DestroyFont();

  prepExit();

  return 0;
}
