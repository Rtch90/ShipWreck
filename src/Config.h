/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

class Config {
public:
  //constructor
  Config(const char *filename);
  
  // Destructor
  ~Config();

  // Make sure the necesary extensions are supported, this needs to happen after we have an openGL context.
  bool checkExtensions();

  // Check to see whether or not to use Vertex Buffer Objects
  bool checkVBO();

  // Returns winWidth/winHeight
  int getWinWidth();
  int getWinHeight();
  
  // check to see whether or not to go fullscreen
  bool checkFullscreen();

  // Check to see whether or not to enable the SDL parachute
  bool checkSDLParachute();

  // Check to see whether or not to compress textures
  bool checkCompressTextures();

  static bool openConfig(const char *filename);
  static void closeConfig();

  static Config *configuration;

private:
  // Window dimensions
  int winWidth;
  int winHeight;

  // True is fullscreen, false is windowed mode.
  bool fullscreen;

  // bool for vertex buffer object usage
  bool useVBO;

  // Boolian to see whether or not we should enable the SDL parachute
  // This should probably be true unless you want to get a core file or something stupid
  bool SDLParachute;

  // Boolian to see whether or not to compress textures on load
  // This should probably always be true, unless your video card can not handle it
  bool CompressTextures;

  // Sending config options to logger
  void LogConfig();
};
