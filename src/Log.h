/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <fstream>
#include <string>

class Log {
public:
  // Construtor
  Log(bool logToFile);
  
  // Deconstructor
  ~Log();
  
  // Log an error message
  void message(std::string msg);
  void message(const char *msg, ...);

  static bool openLog(bool logToFile);
  static void closeLog();

  static Log *logger;

private:
  std::ofstream logFile;
};

#endif
