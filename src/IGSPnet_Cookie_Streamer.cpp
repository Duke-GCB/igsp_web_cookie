#include "IGSPnet_Cookie_Streamer.h"

/* gotta declare the static constants */
const int IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE;
const int IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE;

/*
 * Method Name: buildCookie
 *
 * Description: constructs cookie from first 5 arguments and places in 6th arg.
 *                 Requires cookieString buffer to be preallocated.
 *
 * Arguments  : const char * userID - IGSPnet UserID
 *              const char * dukey - 1 if duke employee, 0 otherwise
 *              const char * IP - IP address of client (www.xxx.yyy.zzz)
 *              const char * cookieVersion - user's cookie version
 *              const char * clientID - 4 character client ID
 *              char * cookieString - buffer to hold the resulting cookie.
 *
 * Returns    : int - 0
 *
 */
int IGSPnet_Cookie_Streamer::buildCookie(const char * userID, const char * dukey, const char * IP, const char * cookieVersion, const char * clientID, char * cookieString)
{
   strcpy(cookieString, userID);
   strcat(cookieString, "::");
   strcat(cookieString, dukey);
   strcat(cookieString, "::");
   strcat(cookieString, IP);
   strcat(cookieString, "::");
   strcat(cookieString, cookieVersion);
   strcat(cookieString, "::");
   strcat(cookieString, clientID);
   return 0;
}

/*
 * Method Name: parseCookie
 *
 * Description: parses contents of cookieString and places components in
 *                 remaining 5 arguments (whose buffers must be preallocated).
 *                 Note that this method destroys cookieString.  
 *
 * Arguments  : char * cookieString - cookie to be parsed.  Contents of
 *                 this argument will be destroyed by this method.
 *              char * userID - IGSPnet UserID
 *              char * dukey - 1 if duke employee, 0 otherwise
 *              char * IP - IP address of client (www.xxx.yyy.zzz)
 *              char * cookieVersion - user's cookie version
 *              char * clientID - 4 character client ID
 *
 * Returns    : int - 0 if cookieString successfully parsed; -1 otherwise.
 *
 */
int IGSPnet_Cookie_Streamer::parseCookie(char * cookieString, char * userID, char * dukey, char * IP, char * cookieVersion, char * clientID)
{
   char * tmp;

   //get userID
   tmp = strstr(cookieString, "::");
   if (tmp == NULL)
      return -1;
   
   *tmp = '\0';
   if (strlen(cookieString) > 12)
      return -1;
   strcpy(userID, cookieString);
   cookieString = tmp + 2;  //skip over the :: delimiter

   //get dukey
   tmp = strstr(cookieString, "::");
   if (tmp == NULL)
      return -1;
   
   *tmp = '\0';
   if (strlen(cookieString) > 1)
      return -1;
   strcpy(dukey, cookieString);
   cookieString = tmp + 2;

   //get IP
   tmp = strstr(cookieString, "::");
   if (tmp == NULL)
      return -1;
   
   *tmp = '\0';
   if (strlen(cookieString) > 15)
      return -1;
   strcpy(IP, cookieString);
   cookieString = tmp + 2;

   //get cookieVersion
   tmp = strstr(cookieString, "::");
   if (tmp == NULL)
      return -1;
   
   *tmp = '\0';
   if (strlen(cookieString) > 1)
      return -1;
   strcpy(cookieVersion, cookieString);
   cookieString = tmp + 2;

   //get clientID
   if (strlen(cookieString) > 4)
      return -1;
   strcpy(clientID, cookieString);
   return 0;
}

/*
 * Method Name: buildSignedCookie
 *
 * Description: combines cookie and signature to produce signedCookie (whose
 *                 buffer must be preallocated).
 *
 * Arguments  : const char * cookie - cookie portion
 *              const char * sig - signature portion
 *              char * signedCookie - buffer to hold signed cookie (must
 *                 be preallocated).
 *
 * Returns    : int - 0
 *
 */
int IGSPnet_Cookie_Streamer::buildSignedCookie(const char * cookie, const char * sig, char * signedCookie)
{
   strcpy(signedCookie, cookie);
   strcat(signedCookie, ":::");
   strcat(signedCookie, sig);
   return 0;
}

/*
 * Method Name: parseSignedCookie
 *
 * Description: parses contents of signedCookie and places cookie and signature
 *                 in remaining 2 arguments (whose buffers must be 
 *                 preallocated).
 *
 * Arguments  : char * signedCookie - signed cookie to be parsed.  Contents of
 *                 this argument will be destroyed by this method.
 *              char * cookie - cookie portion
 *              char * sig - signature portion
 *
 * Returns    : int - 0 if signedCookie successfully parsed; -1 otherwise.
 *
 */
int IGSPnet_Cookie_Streamer::parseSignedCookie(char * signedCookie, char * cookie, char * sig)
{
   char * tmp;

   //get cookie
   tmp = strstr(signedCookie, ":::");
   if (tmp == NULL)
      return -1;
   
   *tmp = '\0';
   strcpy(cookie, signedCookie);
   signedCookie = tmp + 3;  //skip over the ::: delimiter

   strcpy(sig, signedCookie);
   return 0;
}
