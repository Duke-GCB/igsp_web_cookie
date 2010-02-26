#ifndef OCCI_IGSPNET_H
#define OCCI_IGSPNET_H

#include <occi.h>
#include <string>
#include "IGSPnet_Cookie_Config.h"

using namespace oracle::occi;

/*
 * Class Name  : OCCI_IGSPnet
 *
 * Description : Manages connection to Oracle database and provides
 *              methods for managing IGSPnet user cookies.  Uses Oracle
 *              OCCI API.
 *
 * Method Index: OCCI_IGSPnet() - constructor; establishes connection
 *                  to Oracle using DB_CONN_STRING.
 *               ~OCCI_IGSPnet() - destructor; frees memory associated
 *                  with OCCI environment
 *               int checkCookie(const char * userID, const char * IP,
 *                  const char * clientID, const char * cookieVersion) -
 *                  queries database to see if cookie specified by (userID, IP, 
 *                 clientID) is valid and user is enabled and version of this
 *                 cookie is same as user's active version.  If all OK, then
 *                 update softTS of cookie in DB.  Returns 0 if any of the above
 *                 checks fail, or softLifetime of cookie otherwise (for
 *                 resetting client's cookie in browser)
 *               int insertCookie(const char * userID, const char * IP,
 *                  const int hardLifetime, const int softLifetime, 
 *                  char * dukey, char * cookieVersion, char * clientID) - 
 *                  inserts cookie info specified by first four arguments
 *                  into DB and returns DukeEmployee, active cookie version, and
 *                  client ID in remaining three args.  Returns -1 if cookie
 *                  info could not be inserted or 0 otherwise.
 *
 */
class OCCI_IGSPnet
{
   public:
      OCCI_IGSPnet();
      ~OCCI_IGSPnet();
      int checkCookie(const char * userID, const char * IP, const char * clientID, const char * cookieVersion);
      int insertCookie(const char * userID, const char * IP, const int hardLifetime, const int softLifetime, char * dukey, char * cookieVersion, char * clientID);
   private:
      Environment * env;
      Connection * conn;
      Statement * stmtCheckCookie;
      Statement * stmtInsertCookie;
      Statement * stmtPing;
      void cleanupConnection();
      int getConnection(bool throwExceptions = false);
};

#endif
