#include "CookieDaemonConfig.h"
#include <stdio.h>

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