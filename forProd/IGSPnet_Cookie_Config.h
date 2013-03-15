#ifndef IGSPNET_COOKIE_CONFIG
#define IGSPNET_COOKIE_CONFIG
#define SVNPOS "trunk"

/* path to Unix domain socket */
#define SOCKET_PATH "/var/system/cookied/cookieDaemon.sock"
//#define SOCKET_PATH "/var/www/bin/cookieDaemon.sock"
/* Oracle connect string */
#define DB_CONN_STRING "***REMOVED***"
/* Oracle database user to connect with */
#define DB_USER "IGSPNET_COOKIE_DAEMON"
/* Oracle password to connect with */
#define DB_PASS "***REMOVED***"

#define NORMAL_EXIT 0
#define FATAL_EXIT 1
#define USER_EXIT 2


#endif
