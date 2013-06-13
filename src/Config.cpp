/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <cstring>
#include <string.h>
#include <sstream>
#include <iostream>
#include "Log.h"
#include "Config.h"
#include "extensions.h"
#include "tinyxml.h"

using namespace std;

Config *Config::configuration = NULL;

Config::Config(const char *filename) {
  TiXmlDocument doc(filename);
  TiXmlElement* config;
  TiXmlElement* resolution;
  TiXmlElement* temp;
  TiXmlText* data;
  int n;

  winWidth   = 640;
  winHeight  = 480;
  fullscreen = false;
  useVBO     = false;
  SDLParachute = true;
  CompressTextures = true;

    doc.LoadFile();
    if(doc.Error()) {
      Log::logger->message("Warning: %s (line %d) Using defaults", doc.ErrorDesc(), doc.ErrorRow());
      LogConfig();
      return;
    }
    config = doc.FirstChildElement("GameConfiguration"); // This get's the <GameConfiguration> tag
    if(config == NULL) {
      Log::logger->message("Warning: <GameConfiguration> did not parse correctly.");
      LogConfig();
      return;
    }

  try {
    resolution = config->FirstChildElement("Resoution"); // Get <resolution>
    if(resolution == NULL) throw 0;

    // Handle the resolution
    temp = resolution->FirstChildElement("Width");
    if(temp == NULL) throw 0;
    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 1;
    n = sscanf(data->Value(), "%d", &winWidth);
    if(n != 1) throw 1;

    temp = resolution->FirstChildElement("Height");
    if(temp == NULL) throw 0;
    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 2;
    n = sscanf(data->Value(), "%d", &winHeight);
    if(n != 1) throw 2;
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("Warning: <Resolution> did not parse correctly");
      break;
    case 1:
      Log::logger->message("Warning: Invalid <Width> data, using defaults");
      break;
    case 2:
      Log::logger->message("Warning: Invalid <Height> data, using defaults");
      break;
    default:
      // Should not get to here
      Log::logger->message("Warning: Unknown <Resolution> error");
      break;
    }

    // Set it back to the defaults
    winWidth  = 640;
    winHeight = 480;
  }

  try {
    // Handle Fullscreen
    temp = config->FirstChildElement("Fullscreen");
    if(temp == NULL) throw 0;

    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 1;
    if(strcmp(data->Value(), "true") == 0) {
      fullscreen = true;
    } else if(strcmp(data->Value(), "false") == 0) {
      fullscreen = false;       
    } else {
      throw 1;
    }
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("Warning: <Fullscreen> did not parse correctly");
      break;
    case 1:
      Log::logger->message("Warning: Invalid Fullscreen data, using defaults");
      break;
    default:
      // Should not get here
      Log::logger->message("Warning: Unknown <Fullscreen> error");
      break;
    }
  }

  try {
    // Handle Vertex buffer objects
    temp = config->FirstChildElement("VBO");
    if(temp == NULL) throw 0;

    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 1;
    if(strcmp(data->Value(), "true") == 0) {
      useVBO = true;
    } else if(strcmp(data->Value(), "false") == 0) {
      useVBO = false;  
    } else {
      throw 1;
    }
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("Warning: Invalid <VBO> did not parse correctly");
      break;
    case 1:
      Log::logger->message("Warning: Invalid VBO data, using defaults");
      break;
    default:
      // Should not get here
      Log::logger->message("Warning: Unknown <VBO> error");
      break;
    }
  }

  try {
    // Deal with SDLParachute
    temp = config->FirstChildElement("SDLParachute");
    if(temp == NULL) throw 0;

    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 1;
    if(strcmp(data->Value(), "true") == 0) {
      SDLParachute = true;
    } else {
      throw 1;
    }
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("Warning: <SDLParachute> did not parse correctly");
      break;
    case 1:
      Log::logger->message("Warning: Invalid SDLParachute data, using default");
      break;
    default:
      // We should not get here....
      Log::logger->message("Warning: Unknown <SDLParachute> error");
      break;
    }
  }

  try {
    // Deal with CompressTextures
    temp = config->FirstChildElement("CompressTextures");
    if(temp == NULL) throw 0;

    data = temp->FirstChild()->ToText();
    if(data == NULL) throw 1;
    if(strcmp(data->Value(), "false") == 0) {
      CompressTextures = true;
    } else if(strcmp(data->Value(), "false") == 0) {
      CompressTextures = false;
    } else {
      throw 1;
    }
  } catch(int i) {
    switch(i) {
    case 0:
      Log::logger->message("Warning:CompressTextures> did not parse correctly");
      break;
    case 1:
      Log::logger->message("Warning: invalid CompressTexture data, using default");
      break;
    default:
      // Shouldn't get here
      Log::logger->message("Warning: Unknown <CompressTexture> error");
      break;
    }
  }
  
  LogConfig();
}

// Doesn't realy need to do anything.
Config::~Config() {
}

// This needs to happen after we have an openGL context, so it can not happen in the constructor
bool Config::checkExtensions() {
  // This could probably be nicer.
  bool supportMultiTex = checkExtension("GL_ARB_multitexture");
  bool supportVBO = checkExtension("GL_ARB_vertex_buffer_object");

  // Make sure we have support for multitexturing.
  if(!supportMultiTex) {
    // Probably should just throw an exeption.
    Log::logger->message("Error: Your video card is crap and does not support multitexturing");
    return false;
  }
  
  // Make sure we do not try to use vertex buffer objects without support
  if(!supportVBO && useVBO) {
    useVBO = false;
    Log::logger->message("Warning: Your video card does not support VBOs, falling back onto VAs");
  }
  
  // We have multitexturing so we can continue forth.
  return true;
}

bool Config::checkVBO() { 
  return useVBO;
}

int Config::getWinWidth() {
  return winWidth;
}

int Config::getWinHeight() {
  return winHeight;
}

bool Config::checkFullscreen() {
  return fullscreen;
}

bool Config::checkSDLParachute() {
  return SDLParachute;
}

bool Config::checkCompressTextures() {
  return CompressTextures;
}

bool Config::openConfig(const char *filename) {
  if(configuration != NULL) {
    Log::logger->message("Warning: Open config on non-NULL configuration"); 
    return false;
  }
  
  configuration = new Config(filename);
  return true;
}

void Config::closeConfig() {
  if (configuration == NULL) {
    Log::logger->message("Warning: Close config on NULL configuration");
    return;
  }

  delete configuration;
  configuration = NULL;
}    

void Config::LogConfig() {
  ostringstream outputString;

  outputString << "Configuration:" << endl;
  outputString << "Resolution: " << winWidth << " x " << winHeight << endl;
  
  outputString << "Fullscreen: ";
  if(fullscreen) outputString << "true";
  else outputString << "false";
  outputString << endl;

  outputString << "Use VBOs: ";
  if(useVBO) outputString << "true";
  else outputString << "false";
  outputString << endl;
  
  outputString << "Enable SDL Parachute: ";
  if(SDLParachute) outputString << "true";
  else outputString << "false";

  outputString << endl;

  outputString << "Compress Textures: ";
  if(CompressTextures) outputString << "true";
  else outputString << "false";

  Log::logger->message(outputString.str());
}
