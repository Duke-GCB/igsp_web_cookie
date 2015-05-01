#include "CookieDaemonConfig.h"
#include <stdio.h>

/* Simple command-line tool to read a CookieDaemonConfig file and print it out
 * Usage: readconf cookied.conf
 */
int main(int argc, char * argv[]) {
  if(argc > 1) {
    std::string filename(argv[1]);
    CookieDaemonConfig conf(filename);
    conf.print();
    return 0;
  } else {
    fprintf(stderr, "Usage: %s cookied.conf\n", argv[0]);
    return 1;
  }
}
