#ifndef RSA_SIGN_VERIFY_H
#define RSA_SIGN_VERIFY_H

#include <stdio.h>
#include <string>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

/*
 * Class Name  : RSA_Sign_Verify
 *
 * Description : Provides methods to digitally sign a string using an
 *                  RSA private key and verify a string/signature combination
 *                  using an RSA public key.
 *
 * Method Index: static int signString(const char * cookieData, char * hexSig) -
 *                  digitally signs cookieData and places signature in
 *                  hexSig.  Returns 0 if cookieData successfully signed, -1
 *                  otherwise.
 *               static int verifySig(const char * cookieData,
 *                  const char * hexSig) - verifies signature hexSig is valid
 *                  over string cookieData.  Returns 0 if signature is valid,
 *                  -1 otherwise.
 */
class RSA_Sign_Verify
{
   public:
      //uses default constructor and destructor
      static int signString(const char * cookieData, char * hexSig);
      static int verifySig(const char * cookieData, const char * hexSig);
/* Emperically, it appears that the length of the binhex-coded RSA signature 
 * is 4X the length of the private key.  So, for a 2048-bit key, the binhex
 * sig may be 512 characters long.
 * We know the max length of the cookie is 12+2+1+2+15+2+1+2+4 = 41.  There
 * is also a 3 character delimiter (:::) between the cookie and the sig, so
 * the total length of a signed cookie is around 41+3+512 = 556 characters.
 * Let's define some conservative constants, below:
 * SOCKET_RW_BUFFER_SIZE
 * RSA_SIG_BUFFER_SIZE
 */
      /* socket read buffer size - should be no more than a signed cookie */
      static const int SOCKET_RW_BUFFER_SIZE = 1024;  //overestimate

   private:
      static char binToHexMapping(const unsigned char c);
      static unsigned char hexToBinMapping(const char c);
      static void bin2hex(const unsigned char * data, const unsigned int data_len, char * buffer);
      static void hex2bin(const char * data, unsigned char * buffer, unsigned int &buffer_len);
      /* RSA signature, in binary, is 2X length of a signed cookie in hex */
      static const int RSA_SIG_BUFFER_SIZE = 2048;  //overestimate
      
};

#endif
