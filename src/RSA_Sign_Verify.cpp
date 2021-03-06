#include "RSA_Sign_Verify.h"
#include "CookieDaemonConfig.h"

/* gotta declare the static constants */
const int RSA_Sign_Verify::SOCKET_RW_BUFFER_SIZE;
const int RSA_Sign_Verify::RSA_SIG_BUFFER_SIZE;

/*
 * Method Name: signString
 *
 * Description: digitally signs cookieData and places signature in hexSig.
 *
 * Arguments  : const char * cookieData - string to sign
 *              char * hexSig - buffer to hold the signature (must be
 *                 preallocated).
 *
 * Returns    : int - 0 if cookieData successfully signed, -1 otherwise.
 *
 */
int RSA_Sign_Verify::signString(const char * cookieData, char * hexSig)
{
   unsigned int sig_len;
   int err;

   unsigned char sig_buf[RSA_SIG_BUFFER_SIZE];
   EVP_MD_CTX     md_ctx;
   EVP_PKEY *     pkey;

   /* Read private key */
   // Path to key file is in CookieDaemonConfig. Since this method is static,
   // we'll just use the static getConfig() method.
   CookieDaemonConfig *config = CookieDaemonConfig::getConfig();
   if(config == NULL) {
      fprintf(stderr, "No config found, exiting\n");
      return -1;
   }

   FILE *keyFile = fopen(config->getPrivateKeyPath().c_str(), "r");
   // Clean up config early
   if(config != NULL) {
     delete config;
   }
   if(keyFile == NULL) {
     fprintf(stderr, "Can't open private key file %s!\n", config->getPrivateKeyPath().c_str());
     return -1;
   }
   // Use openssl to read the key directly from the pem file.
   pkey = PEM_read_PrivateKey(keyFile, NULL, NULL, NULL);
   // Key in memory, close the file
   fclose(keyFile);

   if (pkey == NULL)
   {
      fprintf(stderr, "Error reading private key.\n");
      return -1;
   }

   /* Do the signature */
   EVP_SignInit   (&md_ctx, EVP_sha1());
   EVP_SignUpdate (&md_ctx, cookieData, strlen(cookieData));
   sig_len = sizeof(sig_buf);
   err = EVP_SignFinal (&md_ctx, sig_buf, &sig_len, pkey);

   if (err != 1)
   {
      fprintf(stderr, "Error signing cookie.\n");
      return -1;
   }

   EVP_PKEY_free (pkey);
   bin2hex (sig_buf, sig_len, hexSig);
   return 0;
}

/*
 * Method Name: verifySig
 *
 * Description: verifies signature hexSig is valid over string cookieData.
 *
 * Arguments  : const char * cookieData - signed string
 *              const char * hexSig - signature.
 *
 * Returns    : int - 0 if signature is valid, -1 otherwise
 *
 */
int RSA_Sign_Verify::verifySig(const char * cookieData, const char * hexSig)
{
   unsigned int sig_len;

   int err;
   unsigned char  sig_buf [RSA_SIG_BUFFER_SIZE];
   EVP_MD_CTX     md_ctx;
   EVP_PKEY *     pubkey;
   X509 *         x509;

   /* Read public key (certificate) */
   // Path to certificate file is in CookieDaemonConfig. Since this method is static,
   // we'll just use the static getConfig() method.
   CookieDaemonConfig *config = CookieDaemonConfig::getConfig();
   if(config == NULL) {
      fprintf(stderr, "No config found, exiting\n");
      return -1;
   }

   FILE *certFile = fopen(config->getCertPath().c_str(), "r");
   // Clean up config early
   if(config != NULL) {
     delete config;
   }
   if(certFile == NULL) {
     fprintf(stderr, "Can't open certificate file %s!\n", config->getCertPath().c_str());
     return -1;
   }

   // Read the cert directly from the pem file.
   x509 = PEM_read_X509(certFile, NULL, NULL, NULL);

   // Cert in memory, close the file
   fclose(certFile);

   if (x509 == NULL)
   {
      fprintf(stderr, "Error reading public key cert.\n");
      return -1;
   }

   /* Get public key - eay */
   pubkey=X509_get_pubkey(x509);
   if (pubkey == NULL)
   {
      fprintf(stderr, "Error getting public key.\n");
      return -1;
   }

   /* Decode the cookie (it's in hex) */
   hex2bin(hexSig, sig_buf, sig_len);

   /* Verify the signature */
   EVP_VerifyInit   (&md_ctx, EVP_sha1());
   EVP_VerifyUpdate (&md_ctx, cookieData, strlen((char*)cookieData));
   err = EVP_VerifyFinal (&md_ctx, sig_buf, sig_len, pubkey);
   EVP_PKEY_free (pubkey);

   if (err != 1)
   {
      fprintf(stderr, "Signature verification failed.\n");
      return -1;
   }

   return 0;
}

char RSA_Sign_Verify::binToHexMapping(const unsigned char c)
{
   if (c < 10)
      return (char) 48 + c;
   else
      return (char) 65 - 10 + c;
}

unsigned char RSA_Sign_Verify::hexToBinMapping(const char c)
{
   if (c <= '9')
      return (unsigned char) c - 48;
   else
      return (unsigned char) c - 65 + 10;
}

void RSA_Sign_Verify::bin2hex(const unsigned char * data, const unsigned int data_len, char * buffer)
{
   unsigned char a;
   unsigned char b;

   for (unsigned int i = 0; i < data_len; i++)
   {
      a = data[i] / 16;
      b = data[i] % 16;
      buffer[i+i] = binToHexMapping(a);
      buffer[i+i+1] = binToHexMapping(b);
   }

   buffer[data_len + data_len] = 0;
}

void RSA_Sign_Verify::hex2bin(const char * data, unsigned char * buffer, unsigned int &buffer_len)
{
   unsigned i = 0;

   while (data[i+i] != 0)
   {
      buffer[i] = (16 * hexToBinMapping(data[i+i])) + (hexToBinMapping(data[i+i+1]));
      i++;
   }

   buffer_len = i;
}

