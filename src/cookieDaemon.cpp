/* cookieDaemon.cpp
 *
 * Listens on Unix domain socket for cookies from verifyCookie clients.  Parses
 * cookie contents and verifies user's enabled status and soft/hardTimestamps
 * in IGSPnet.
 *  
 * Runs as a daemon process.  Any errors are logged to stderr; fatal errors
 * exit with -1.  SIGHUP, SIGINT, SIGTERM are trapped and return 0.
 *
 */
 
/* cookie format = userID::dukey::IP::cookieVersion::clientID:::sig */

#include "cookieDaemon.h"

/* listener socket must be close-able by signal handler,
 * so must be global */
int l; //listener socket handle
OCCI_IGSPnet *db = NULL;  //db handler must be freed on exit
CookieDaemonConfig *config = NULL;
const char * socket_path() {
  if(config == NULL) {
    config = CookieDaemonConfig::getConfig();
  }
  
  return config->getSocketPath().c_str();
}

/*
 * Function Name: cleanup
 *
 * Description  : closes listener socket, if open
 *
 * Arguments    : int signum - signal number.  Ignored, but required by
 *                   signal.h API.
 *
 * Returns      : None
 *
 */
void cleanup(int signum)
{
   close(l);
   unlink(socket_path());

   if (db != NULL)
   {
      delete db;
      db = NULL;
   }
   delete(config);
   config = NULL;
   
   //if we got here, assume normal termination
   exit (NORMAL_EXIT);
}

/*
 * Function Name: main
 *
 * Description  : program entry point
 *
 * Arguments    : None
 *
 * Returns      : 0 in theory, but exit only ever actually occurs following
 *                   fatal error (-1) or signal (SIGHUG, SIGINT, SIGTERM) (0).
 *
 */
int main(int argc, char * argv[])
{
   int w;   /* listener and worker sockets */
   struct sockaddr_un sa;   /* socket address */
   int count;  /* length of stream read by socket */
   char buffer[RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE]; /* socket read/write buffer */
   
   /* set up signal handlers */
   signal(SIGHUP, cleanup);
   signal(SIGINT, cleanup);
   signal(SIGTERM, cleanup);

   /* create listener socket */
   l = socket(AF_UNIX, SOCK_STREAM, 0);
   if (l < 0)
   {
      fprintf(stderr, "socket(): Cannot create listener socket - %s\n", strerror(errno));
      return FATAL_EXIT;
   }

   /* initialize sockaddr_in struct */
   bzero(&sa, sizeof (struct sockaddr_un));
   sa.sun_family = AF_UNIX;
   strcpy(sa.sun_path, socket_path());

   /* bind listener socket  */
   unlink(socket_path());  /* in case it already exists from prior run */
   if (bind(l, (struct sockaddr *) &sa, sizeof (sa)) < 0)
   {
      fprintf(stderr, "bind(): Cannot bind socket to %s - %s\n", socket_path(), strerror(errno));
      return FATAL_EXIT;
   }

   /* listening */
   if (listen(l, 5) < 0)
   {
      fprintf(stderr, "listen(): Cannot listen on socket - %s\n", strerror(errno));
      return FATAL_EXIT;
   }
   
   fprintf(stderr, "Listening on socket (bound to %s)\n", socket_path()); 

   char responseBuffer[RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE];
   
   /* enable us to talk to verify signatures and talk w/ Oracle */
   try
   {
      db = new OCCI_IGSPnet();  //die if can't connect
   }
   catch (SQLException &e)
   {
      fprintf(stderr, "OCCI_IGSPnet(): Can't connect to database - %s\n", e.what());
      return FATAL_EXIT;
   }

   while (1)
   {
      /* accept connection; pass to worker */
      w = accept(l, NULL, NULL);
      if (w < 0)
      {
         fprintf(stderr, "accept(): Error accepting on socket - %s\n", strerror(errno));
         close(w);
         continue;
      }
      
      /* read up to READ_BUFFER_SIZE-1 bytes - if more, close the socket */
      count = read(w, buffer, RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE - 1);
      
      if (count < 0)
      {
         fprintf(stderr, "read(): Error reading socket - %s\n", strerror(errno));
         close(w);
         continue;
      }
      else
      if (count >= RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE - 1) /* buffer overflow */
      {
         fprintf(stderr, "read(): Buffer full reading socket; discarding\n");
         close(w);
         continue;
      }
      else /* all is well */
      { 
         buffer[count] = '\0'; /* buffer now has the cookie text */

         /* userID::dukey::IP::cookieVersion::clientID */
         /* verify the cookie and send back result */

         /* These buffers are maximum possible for a valid cookie. */
         /* parseCookie() checks these to prevent overflow. */
         char userID[13];
         char dukey[2];
         char IP[16];
         char cookieVersion[2];
         char clientID[5];
         int failure = 0;

         if (IGSPnet_Cookie_Streamer::parseCookie(buffer, userID, dukey, IP, cookieVersion, clientID) != 0)
         {
            fprintf(stderr, "parseCookie(): Could not parse cookie data\n");
            strcpy(responseBuffer, "0");  //failed response
            failure = 1;
         }
         int shortLifetime;
         if (!failure)
         {
            shortLifetime = db->checkCookie(userID, IP, clientID, cookieVersion);
            //fprintf(stderr, "responseBuffer = %d\n", shortLifetime);
            sprintf(responseBuffer, "%d", shortLifetime);
         }
         
         /* responseBuffer contains the response */

         /* write the results here */

         if (write(w, responseBuffer, strlen(responseBuffer)) < 0)
         {
            fprintf(stderr, "write(): Error writing socket - %s\n", strerror(errno));
         }

         //* and then close the socket */
      
         close(w);
      }
   }
  
  /* we'll never get here b/c of the infinite while.  Program exit
   * actually occurs from cleanup(), which returns 0, but main() should
   * return something, so return 0.
   */
   cleanup(0);  //0 is used as dummy sig handler
   return NORMAL_EXIT;
}
