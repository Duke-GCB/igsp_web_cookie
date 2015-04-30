#include <stdio.h>
#include "RSA_Sign_Verify.h"
#include "OCCI_IGSPnet.h"
#include "IGSPnet_Cookie_Streamer.h"
#include "CookieDaemonConfig.h"

void printUsage(char * programName)
{
   fprintf(stderr, "USAGE: %s <UserID> <IP> <softLifetime> <hardLifetime>\n", programName);
   fprintf(stderr, "where: UserID       = IGSPnet UserID (1-12 characters)\n");
   fprintf(stderr, "       IP           = IP address of client (www.xxx.yyy.zzz)\n");
   fprintf(stderr, "       softLifetime = lifetime, in seconds, of cookie in user's browser\n");
   fprintf(stderr, "       hardLifetime = absolute lifetime, in seconds, of cookie\n");
   fprintf(stderr, "       hardLifetime >= softLifetime\n");
   exit(FATAL_EXIT);
}

int main(int argc, char * argv[])
{
   if (argc != 5)
   {
     printUsage(argv[0]);
   }

   char userID[13];
   char IP[16];
   int softLifetime;
   int hardLifetime;
   
   if ((strlen(argv[1]) > 12) || (strlen(argv[2])) > 15)
      printUsage(argv[0]);
   
   strcpy(userID, argv[1]);
   strcpy(IP, argv[2]);
   softLifetime = atoi(argv[3]);
   hardLifetime = atoi(argv[4]);

   if ((softLifetime <= 0) || (hardLifetime <= 0) || (hardLifetime < softLifetime))
      printUsage(argv[0]);
   
   OCCI_IGSPnet *odb = NULL;
   char dukey[2];
   char cookieVersion[2];
   char clientID[5];
   
   try
   {
      odb = new OCCI_IGSPnet();  //die if can't connect
   }
   catch (SQLException &e)
   {
      fprintf(stderr, "OCCI_IGSPnet(): Can't connect to database - %s\n", e.what());
      exit(FATAL_EXIT);
   }
   
   if (odb->insertCookie(userID, IP, hardLifetime, softLifetime, dukey, cookieVersion, clientID) != 0)
   {
      //user not enabled or lifetime invalid
      fprintf(stderr, "insertCookie(): cannot insert cookie\n");
      delete odb;  //free the memory
      exit(USER_EXIT);
   }
   
   delete odb;  //done with db connection
   
   char cookieText[IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE];
   char signatureText[IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE];
   char rval[IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE + IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE + 3];  //3 for delimiter
   IGSPnet_Cookie_Streamer::buildCookie(userID, dukey, IP, cookieVersion, clientID, cookieText);
   if (RSA_Sign_Verify::signString(cookieText, signatureText) != 0)
   {
      fprintf(stderr, "signString(): cannot sign cookie\n");
      exit(FATAL_EXIT);  //cannot sign
   }
   
   IGSPnet_Cookie_Streamer::buildSignedCookie(cookieText, signatureText, rval);
   
   printf("%s", rval);
   return NORMAL_EXIT;
}
