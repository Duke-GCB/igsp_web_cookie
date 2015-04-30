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

#include <iostream>

class CookieDaemonConfig {
  public:
    CookieDaemonConfig(std::string filename);
    void print();
    std::string getSocketPath() { return socket_path; }
    std::string getConnectionString() { return db_conn_string; }
    std::string getDBUser() { return db_user; }
    std::string getDBPass() { return db_pass; }
  private:
    void setValue(std::string key, std::string value);
    void readFile(std::string filename);
    std::string socket_path;
    std::string db_conn_string;
    std::string db_user;
    std::string db_pass;
};

#endif