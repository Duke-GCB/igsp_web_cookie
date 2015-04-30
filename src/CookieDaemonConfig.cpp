#include "CookieDaemonConfig.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CookieDaemonConfig::CookieDaemonConfig(std::string filename) {
  readFile(filename);
}

/*
  Build a CookieDaemonConfig from the file located at COOKIE_DAEMON_CONFIG,
  or the default path if COOKIE_DAEMON_CONFIG not set in the environment.
  Caller is responsible for deleting the object
*/
CookieDaemonConfig * CookieDaemonConfig::getConfig() {
  char * path = getenv(CONFIG_ENV);
  if(!path) {
    strcpy(path, DEFAULT_CONFIG_PATH);
  }
  CookieDaemonConfig *config = new CookieDaemonConfig(path);
  return config;
}

void CookieDaemonConfig::readFile(std::string filename) {
  std::ifstream infile(filename.c_str());
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::string k, v;
    if (!(iss >> k >> v)) { break; } // error
    setValue(k,v);
  }
}

void CookieDaemonConfig::setValue(std::string key, std::string value) {
  if(key.compare("SOCKET_PATH") == 0) {
    socket_path = value;
  } else if(key.compare("DB_CONN_STRING") == 0) {
    db_conn_string = value;
  } else if(key.compare("DB_USER") == 0) {
    db_user = value;
  } else if(key.compare("DB_PASS") == 0) {
    db_pass = value;
  }
}

void CookieDaemonConfig::print() {
  printf("Socket path: %s\n", socket_path.c_str());
  printf("Connection string: %s\n", db_conn_string.c_str());
  printf("User: %s\n", db_user.c_str());
  printf("Password: %s\n", db_pass.c_str());
}
