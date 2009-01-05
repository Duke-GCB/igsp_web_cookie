#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "RSA_Sign_Verify.h"
#include "IGSPnet_Cookie_Streamer.h"
#include "IGSPnet_Cookie_Config.h"

void printUsage(char * programName)
{
   fprintf(stderr, "USAGE: %s <signedCookie> [<IP>]\n", programName);
   fprintf(stderr, "where: signedCookie = IGSPnet cookie, digitally signed, as produced by signCookie\n");
   fprintf(stderr, "       IP           = optional IP address of client (www.xxx.yyy.zzz)\n");
   exit(FATAL_EXIT);
}

int main(int argc, char * argv[])
{
   char myIP[16];
   myIP[0] = '\0';

   if ((argc != 2) && (argc != 3)) 
      printUsage(argv[0]);

   if (argc == 3)
   {
      if (strlen(argv[2]) > 16)
         printUsage(argv[0]);
      strcpy(myIP, argv[2]);
   }  
   
   char signedCookie[IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE + IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE + 3];  //3 for delimiter
   int softLifetime;
   
   if (strlen(argv[1]) > (IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE + IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE + 3))
   {
      fprintf(stderr, "signed cookie is invalid\n");
      exit(USER_EXIT);
   }
   
   strcpy(signedCookie, argv[1]);
   char cookieText[IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE];
   char cookieTextForParse[IGSPnet_Cookie_Streamer::IGSPNET_COOKIE_SIZE];
   char signatureText[IGSPnet_Cookie_Streamer::RSA_HEX_SIG_SIZE];
   char userID[13];  //not used
   char dukey[2];    //not used
   char IP[16];
   char cookieVersion[2];  //not used
   char clientID[5];  //not used

   if (IGSPnet_Cookie_Streamer::parseSignedCookie(signedCookie, cookieText, signatureText) != 0)
   {
      fprintf(stderr, "parseSignedCookie(): cannot parse signed cookie\n");
      exit(USER_EXIT);
   }
   if (RSA_Sign_Verify::verifySig(cookieText, signatureText) != 0)
   {
      fprintf(stderr, "verifySig(): cannot verify digital signature\n");
      exit(USER_EXIT);
   }
   if (strlen(myIP) > 0)
   {
      strcpy(cookieTextForParse, cookieText);
      if (IGSPnet_Cookie_Streamer::parseCookie(cookieTextForParse, userID, dukey, IP, cookieVersion, clientID) != 0)
      {
         fprintf(stderr, "parseCookie(): cannot parse cookie data\n");
         exit(USER_EXIT);
      }
      if (strcmp(myIP, IP) != 0)
      {
         fprintf(stderr, "parseCookie(): IP check failure\n");
         exit(USER_EXIT);
      }
   }

   //now time to connect to cookieDaemon
   int s;
   struct sockaddr_un sa;
   char buffer[RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE];
   int count;
   
   if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
   {
      fprintf(stderr, "socket(): could not open socket");
      exit(FATAL_EXIT);
   }

   sa.sun_family = AF_UNIX;
   strcpy(sa.sun_path, SOCKET_PATH);

   if (connect(s, (struct sockaddr *) &sa, sizeof (sa)) < 0)
   {
      fprintf(stderr, "connect(): could not connect to socket %s", SOCKET_PATH);
      exit(FATAL_EXIT);
   }

   if (send(s, cookieText, strlen(cookieText), 0) < 0)
   {
      fprintf(stderr, "send(): error sending cookie to daemon\n");
      exit(FATAL_EXIT);
   }
   
   count = recv(s, buffer, RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE - 1, 0);
   if (count > 0)
      buffer[count] = '\0';
   else if (count < 0)
   {
      fprintf(stderr, "recv(): error receiving from daemon\n");
      exit(FATAL_EXIT);
   }
   else
   {
      fprintf(stderr, "server closed connection\n");
      exit(FATAL_EXIT);
   }
   
   close(s);
   
   //now, buffer has the response from the daemon
   printf("%s", buffer);
   return NORMAL_EXIT;
}
