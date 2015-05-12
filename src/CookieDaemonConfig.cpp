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
  const char * path = getenv(CONFIG_ENV);
  if(!path) {
    path = DEFAULT_CONFIG_PATH;
  }
  CookieDaemonConfig *config = new CookieDaemonConfig(path);
  if(config->isValid()) {
    return config;
  } else {
      fprintf(stderr, "getConfig(): No config read.\nEither set %s with path to valid config or place config in %s\n", CONFIG_ENV, DEFAULT_CONFIG_PATH);
    return NULL;
  }
}
/* Populate values from a named file. File is interpreted as
 * space-separated keys and values, one per line.
 */
void CookieDaemonConfig::readFile(std::string filename) {
  std::ifstream infile(filename.c_str());
  std::string k, v;
  while (infile >> k >> v) {
    setValue(k,v);
  }
}

/* Config objects are only valid if they have nonzero values for all members */
bool CookieDaemonConfig::isValid() {
  return (socket_path.length() > 0
    && db_conn_string.length() > 0
    && db_user.length() > 0
    && db_pass.length() > 0
    && private_key_path.length() > 0
    && cert_path.length() > 0);
}

/* Populate member variables by key */
void CookieDaemonConfig::setValue(std::string key, std::string value) {
  if(key.compare("SOCKET_PATH") == 0) {
    socket_path = std::string(value);
  } else if(key.compare("DB_CONN_STRING") == 0) {
    db_conn_string = std::string(value);
  } else if(key.compare("DB_USER") == 0) {
    db_user = std::string(value);
  } else if(key.compare("DB_PASS") == 0) {
    db_pass = std::string(value);
  } else if(key.compare("PRIVATE_KEY_PATH") == 0) {
    private_key_path = std::string(value);
  } else if(key.compare("CERT_PATH") == 0) {
    cert_path = std::string(value);
  }
}

/* Helper method for printing a CookieDaemonConfig.
 * For true C++, should override operator<<
*/
void CookieDaemonConfig::print() {
  printf("Socket path: %s\n", socket_path.c_str());
  printf("Connection string: %s\n", db_conn_string.c_str());
  printf("User: %s\n", db_user.c_str());
  printf("Password: %s\n", db_pass.c_str());
  printf("Private Key Path: %s\n", private_key_path.c_str());
  printf("Certificate Path: %s\n", cert_path.c_str());
}

/* Accessors */
std::string CookieDaemonConfig::getSocketPath() { return socket_path; }
std::string CookieDaemonConfig::getConnectionString() { return db_conn_string; }
std::string CookieDaemonConfig::getDBUser() { return db_user; }
std::string CookieDaemonConfig::getDBPass() { return db_pass; }
std::string CookieDaemonConfig::getPrivateKeyPath() { return private_key_path; }
std::string CookieDaemonConfig::getCertPath() { return cert_path; }
