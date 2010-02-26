#ifndef IGSPNET_COOKIE_STREAMER_H
#define IGSPNET_COOKIE_STREAMER_H

#include <string.h>

/*
 * Class Name  : IGSPnet_Cookie_Streamer
 *
 * Description : Provides methods for building and parsing cookie contents
 *
 * Method Index: static int buildCookie(const char * userID, const char * dukey,
 *                  const char * IP, const char * cookieVersion, 
 *                  const char * clientID, char * cookieString) - constructs
 *                  cookie from first 5 arguments and places in 6th arg.
 *                  Requires cookieString buffer to be preallocated.
 *                  Returns 0.
 *               static int parseCookie(char * cookieString, char * userID,
 *                  char * dukey, char * IP, char * cookieVersion, 
 *                  char * clientID) - parses contents of cookieString and
 *                  places components in remaining 5 arguments (whose buffers
 *                  must be preallocated).  Note that this method destroys
 *                  cookieString.  Returns 0 if successful or -1
 *                  if cookieString cannot be parsed.
 *               static int buildSignedCookie(const char * cookie, 
 *                  const char * sig, char * signedCookie) - combines
 *                  cookie and signature to produce signedCookie (whose
 *                  buffer must be preallocated).  Returns 0.
 *               static int parseSignedCookie(char * signedCookie,
 *                  char * cookie, char * sig) - parses cookie and sig from
 *                  signedCookie.  Requires cookie and sig buffers to be
 *                  preallocated.  Note that this method destroys signedCookie.
 *                  Returns 0 if successful or -1 otherwise.
 *
 */
class IGSPnet_Cookie_Streamer
{
   public:
      //uses default constructor and destructor
      static int buildCookie(const char * userID, const char * dukey, const char * IP, const char * cookieVersion, const char * clientID, char * cookieString);
      static int parseCookie(char * cookieString, char * userID, char * dukey, char * IP, char * cookieVersion, char * clientID);
      static int buildSignedCookie(const char * cookie, const char * sig, char * signedCookie);
      static int parseSignedCookie(char * signedCookie, char * cookie, char * sig);
/* Emperically, it appears that the length of the binhex-coded RSA signature 
 * is 4X the length of the private key.  So, for a 2048-bit key, the binhex
 * sig may be 512 characters long.
 * We know the max length of the cookie is 12+2+1+2+15+2+1+2+4 = 41.
 * Let's define some conservative constants, below:
 * IGSPNET_COOKIE_SIZE
 * RSA_HEX_SIG_SIZE
 */
      /* unsigned portion of IGSPnet cookie max size */
      static const int IGSPNET_COOKIE_SIZE = 50;  //overestimate
      /* max lenth of hex RSA sig */
      static const int RSA_HEX_SIG_SIZE = 640;  //overestimate
};

#endif
