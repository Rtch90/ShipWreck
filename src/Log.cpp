/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdarg>
#include "Log.h"

using namespace std;

Log *Log::logger = NULL;

Log::Log(bool logToFile) {
  time_t timestamp;

  if(logToFile) {
    logFile.open("Lephisto.log", ios::out);
    if(!logToFile) {
      // We can not open log file.
      cerr << "Warning: Cannot open error.log for write, continuing without logging\n";
    } else {
      // Log file is open, let's give it a nice time stamp.
      timestamp = time(NULL);
      logFile << "Log Started: " << ctime(&timestamp) << endl;
    }
  }
}

Log::~Log() {
  time_t timestamp;

  // We only realy need to close logFile if it is already open.
  if(logFile) {
    // Give it a pretty closeing timestamp
    timestamp = time(NULL);
    logFile << endl << "Log Closed: " << ctime(&timestamp) << endl;
    
    // Close log file
    logFile.close();
  } 
}

void Log::message(std::string msg) {
  if(logFile) {
    logFile << msg << endl << endl;
  }

  cerr << msg << endl << endl;
}

void Log::message(const char *msg, ...) {
  va_list vargList; // This is to handlle the variable arguments
  
  char outBuf[1024];
  unsigned short outLen;
  
  // This takes the arguments and puts them into the character array.
  va_start(vargList, msg);

#if defined WIN32
  outLen = _vsnprintf(outBuf, sizeof(outBuf), msg, vargList);
#else
  outLen = vsnprintf(outBuf, sizeof(outBuf), msg, vargList);
#endif

  va_end(vargList);

  if(outLen >= sizeof(outBuf)) {
    outLen = sizeof(outBuf);
  }

  if(logFile) {
    logFile << outBuf << endl;
  }

  cerr << outBuf << endl;
}

bool Log::openLog(bool logToFile) {
  // Make sure the logger has not already been initialized.
  if(logger != NULL) {
    logger->message("Warning: Multiple calls to OpenLog()");
    return false;
  }

  logger = new Log(logToFile);
  return true;
}

void Log::closeLog() {
  if(logger == NULL) {
    cerr << "Warning: Call to closeLog with NULL logger pointer" << endl;
    return;
  }

  delete logger;
  logger = NULL;
}
