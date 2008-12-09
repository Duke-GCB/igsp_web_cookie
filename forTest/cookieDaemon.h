/* cookieDaemon.h
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

#ifndef COOKIEDAEMON_H
#define COOKIEDAEMON_H

#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>
#include "OCCI_IGSPnet.h"
#include "RSA_Sign_Verify.h"
#include "IGSPnet_Cookie_Streamer.h"
#include "IGSPnet_Cookie_Config.h"

/*
 * Function Name: cleanup
 *
 * Description  : closes listener socket, if open
 *
 * Arguments    : int signum - signal number.  Ignored, but required by
 *                   signal.h API.
 *
 * Returns      : None
 */
void cleanup(int signum);

#endif  /* COOKIEDAEMON_H */
