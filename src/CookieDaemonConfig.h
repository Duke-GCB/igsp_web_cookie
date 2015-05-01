/* CookieDaemonConfig.h
 *
 * Reads a config file and stores values for
 * cookieDaemon's connection to an oracle database.
 *
 * Construct with a path to a config file
 */

/* Example config:
SOCKET_PATH /path/to/cookieDaemon.sock
DB_CONN_STRING //127.0.0.1:1521/MYSID
DB_USER username
DB_PASS password
*/

#ifndef COOKIE_DAEMON_CONFIG_H
#define COOKIE_DAEMON_CONFIG_H

// Exit codes
#define NORMAL_EXIT 0
#define FATAL_EXIT 1
#define USER_EXIT 2

// Environment variable to read for config file location
#define CONFIG_ENV "COOKIE_DAEMON_CONFIG"

// Default path to use if environment variable not set
#define DEFAULT_CONFIG_PATH "/var/system/cookied/cookied.conf"

#include <iostream>

class CookieDaemonConfig {
  public:
    CookieDaemonConfig(std::string filename);
    ~CookieDaemonConfig();
    static CookieDaemonConfig * getConfig();
    void print();
    std::string getSocketPath() { return *socket_path; }
    std::string getConnectionString() { return *db_conn_string; }
    std::string getDBUser() { return *db_user; }
    std::string getDBPass() { return *db_pass; }
  private:
    void setValue(std::string key, std::string value);
    void readFile(std::string filename);
    bool isValid();
    std::string *socket_path;
    std::string *db_conn_string;
    std::string *db_user;
    std::string *db_pass;
};

#endif