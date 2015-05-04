#include "OCCI_IGSPnet.h"
#include <stdio.h>
#include <stdexcept>

/*
 * Method Name: OCCI_IGSPnet
 *
 * Description: Class constructor.  Establishes connection to
 *    Oracle database and prepares SQL.
 *
 * Arguments  : none
 *
 * Returns    : none
 */
OCCI_IGSPnet::OCCI_IGSPnet()
: env(NULL), conn(NULL), stmtCheckCookie(NULL), stmtInsertCookie(NULL)
{
   // creates default OCCI environment (http://download.oracle.com/docs/cd/B12037_01/appdev.101/b10778/toc.htm)
   env = Environment::createEnvironment(Environment::DEFAULT);
   // Instantiate an object with database connection parameters. Die fatally if null
   config = CookieDaemonConfig::getConfig();
   if(config == NULL) {
      throw std::runtime_error("No config found");
   }
   //set up the connection; fatally die if cannot
   getConnection(true);
}

/*
 * Method Name: ~OCCI_IGSPnet
 *
 * Description: Class destructor.  Frees memory allocated by OCCI
 *    environment and prepared statement handles.
 *
 * Arguments  : none
 *
 * Returns    : none
 *
 */
OCCI_IGSPnet::~OCCI_IGSPnet()
{
   cleanupConnection();
   // free memory allocated by OCCI environment
   if (env != NULL)
      Environment::terminateEnvironment(env);
   // Free up the config object
   delete(config);
   config = NULL;
}

/*
 * Method Name: checkCookie
 *
 * Description: queries database to see if cookie specified by (userID, IP, 
 *                 clientID) is valid and user is enabled and version of this
 *                 cookie is same as user's active version.  If all OK, then
 *                 update softTS of cookie in DB.
 *
 * Arguments  : const char * userID - IGSPnet UserID of cookie to check
 *              const char * IP - IP of cookie to check
 *              const char * clientID - 4 character client ID of cookie to check
 *              const char * cookieVersion - version of this cookie
 *
 * Returns    : int - 0 if any of the above checks fail, or softLifetime of
 *                 cookie otherwise (for resetting client's cookie in browser)
 *
 */
int OCCI_IGSPnet::checkCookie(const char * userID, const char * IP, const char * clientID, const char * cookieVersion)
{
   int shortLifetime;
   
   fprintf(stderr, "Getting connection in checkCookie\n");
   
   if (!getConnection())
      return 0;  //cannot establish connection
   
   stmtCheckCookie->setString(1, userID);
   stmtCheckCookie->setString(2, IP);
   stmtCheckCookie->setString(3, clientID);
   stmtCheckCookie->setString(4, cookieVersion);
   stmtCheckCookie->registerOutParam(5, OCCIINT, sizeof(shortLifetime));
   stmtCheckCookie->executeUpdate();
   conn->commit();

   shortLifetime = stmtCheckCookie->getInt(5);
   
   return shortLifetime;  //0 indicates failure
}

/*
 * Method Name: insertCookie
 *
 * Description: inserts cookie info specified by first four arguments
 *                 into DB and returns DukeEmployee, active cookie version, and
 *                 client ID.
 *
 * Arguments  : const char * userID - IGSPnet UserID of cookie to insert
 *              const char * IP - IP of cookie to insert
 *              const int hardLifetime - hard lifetime in seconds of cookie
 *                 to insert
 *              const int softLifetime - soft lifetime in seconds of cookie
 *                 to insert
 *              char * dukey - 1 if user is a Duke employee; 0 otherwise
 *              char * cookieVersion - user's active cookie version
 *              char * clientID - random 4 character string to ID client
 *
 * Returns    : -1 if cookie could not be inserted (e.g., user does not
 *                 exist, user is disabled, hardLifetime < softLifetime)
 *                 or 0 otherwise
 */
int OCCI_IGSPnet::insertCookie(const char * userID, const char * IP, const int hardLifetime, const int softLifetime, char * dukey, char * cookieVersion, char * clientID)
{
   std::string dbDukey;
   std::string dbCookieVersion;
   std::string dbClientID;
   
   if (!getConnection())
      return -1;  //cannot establish connection


   stmtInsertCookie->setString(1, userID);
   stmtInsertCookie->setString(2, IP);
   stmtInsertCookie->setInt(3, hardLifetime);
   stmtInsertCookie->setInt(4, softLifetime);
   stmtInsertCookie->registerOutParam(5, OCCISTRING, sizeof(dbDukey));
   stmtInsertCookie->registerOutParam(6, OCCISTRING, sizeof(dbCookieVersion));
   stmtInsertCookie->registerOutParam(7, OCCISTRING, sizeof(dbClientID));
   stmtInsertCookie->execute();  //prepared in constructor
   conn->commit();
   
   strcpy(dukey, (stmtInsertCookie->getString(5)).c_str());
   strcpy(cookieVersion, (stmtInsertCookie->getString(6)).c_str());
   strcpy(clientID, (stmtInsertCookie->getString(7)).c_str());
   if (strlen(dukey) > 0)
      return 0;
   else
      return -1;  //cannot insert cookie
}

void OCCI_IGSPnet::cleanupConnection()
{
   // free resources tied up by prepared statements
   try
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): terminating stmtCheckCookie...\n");
      if ((conn != NULL) && (stmtCheckCookie != NULL))
         conn->terminateStatement(stmtCheckCookie);
   }
   catch (...)
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): caught exception terminating stmtCheckCookie\n");
   }
   
   try
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): terminating stmtInsertCookie...\n");
      if ((conn != NULL) && (stmtInsertCookie != NULL))
         conn->terminateStatement(stmtInsertCookie);
   }
   catch (...)
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): caught exception terminating stmtInsertCookie\n");
   }

   // kill the connection
   try
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): terminating conn...\n");
      if ((env != NULL) && (conn != NULL))
         env->terminateConnection(conn);
   }
   catch (...)
   {
      printtime();
      fprintf(stderr, "cleanupConnection(): caught exception terminating conn\n");
   }

   stmtCheckCookie = NULL;
   stmtInsertCookie = NULL;
   conn = NULL;

   return;
}

bool OCCI_IGSPnet::pingConnection() {
  bool success = false;
  if(conn == NULL) {
    // conn must be valid, return early
    return success;
  }
  Statement *stmtPing = NULL;
  ResultSet *rs = NULL;
  try {
    stmtPing = conn->createStatement("SELECT 1 FROM dual");
    rs = stmtPing->executeQuery();
    if (rs->next()) {
      printtime();
      fprintf(stderr, "pingConnection(): rs->next() executed successfully, so ping was good\n");
      success = true;
    } else {
      printtime();
      fprintf(stderr, "pingConnection(): rs->next() was false on ping after creating new connection. This should not happen\n");
    }
  } catch (...) {
    printtime();
    fprintf(stderr, "pingConnection(): Caught exception pinging\n");
  }
  // Cleanup
  if(stmtPing != NULL) {
    if(rs != NULL) {
      stmtPing->closeResultSet(rs);
      rs = NULL;
    }
  conn->terminateStatement(stmtPing);
  stmtPing = NULL;
  }
  return success;
}


int OCCI_IGSPnet::getConnection(bool throwExceptions)
{
   ResultSet *rs;

   try
   {
      if ((conn != NULL) )
      {
         printtime();
         fprintf(stderr, "getConnection(): Connection exists, executing ping...\n");
         bool pingSucceeded = pingConnection();
         if (pingSucceeded) {
            // Ping succeeded, return 1 to indicate connection is OK
            fprintf(stderr, "getConnection(): pingConnection() was true on existing connection, returning");
            return 1;
         } else {
            // Ping failed, need
            fprintf(stderr, "getConnection(): rs->next() was false. Will try to recover...\n");
         }

         if (rs->next())
         {
            printtime();
            fprintf(stderr, "getConnection(): rs->next() was true on test connection, returning with existing connection\n");
            rs->cancel();  //discard the resultset
            return 1;  //the connection is good
         } else {
            printtime();
            fprintf(stderr, "getConnection(): rs->next() was false. Will try to recover...\n");
         }
      }
   }
   catch (...)
   {
         printtime();
         fprintf(stderr, "getConnection(): CAUGHT EXCEPTION BUT DID NOTHING\n");
   
   }
   printtime(); 
   fprintf(stderr, "getConnection(): After ping try block\n");
   
   //if we're here, we don't have a valid connection to the db, so
   //let's try to set one up
   printtime();
   fprintf(stderr, "getConnection(): calling cleanupConnection\n");
   
   cleanupConnection();
   
   try
   {
     printtime();
     fprintf(stderr, "getConnection(): Creating connection\n");
      // connects to DB
      conn = env->createConnection(config->getDBUser(), config->getDBPass(), config->getConnectionString());
   
     printtime();
     fprintf(stderr, "getConnection(): creating statements\n");
      //prepare the statements
      stmtCheckCookie = conn->createStatement("BEGIN IGSPNET2.CHECK_COOKIE(:1, :2, :3, :4, :5); END;");
      stmtInsertCookie = conn->createStatement("BEGIN IGSPNET2.INSERT_COOKIE(:1, :2, :3, :4, :5, :6, :7); END;");
      
     printtime();
     fprintf(stderr, "getConnection(): calling pingConnection()\n");
     if(pingConnection()) {
        printtime();
        fprintf(stderr, "getConnection(): pingConnection() was true on newly created connection\n");
        return 1;
     } else {
        printtime();
        fprintf(stderr, "getConnection(): pingConnection() returned false after creating new connection. This should not happen\n");
     }
      
      //we should not be able to get here, but for completeness, return 0
      return 0;
   }
   catch (SQLException &e)
   {
     printtime();
     fprintf(stderr, "Caught exception %s\n", e.what());
   
      /* Plausible exceptions = ORA-01034, 12541, 03113 */
      if (throwExceptions)
      {
         throw;
      }
      else
      {
         return 0;  //we could not establish a connection
      }
   }
}

