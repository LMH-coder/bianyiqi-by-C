#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser_driver.h"

// CLI: ./jsparser file.js
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file.js>\n", argv[0]);
    return 2;
  }
  int rc = parse_file(argv[1]);
  if (rc == 0) {
    printf("OK\n");
    return 0;
  } else if (rc == 1) {
    // error already printed by parse_file
    return 1;
  } else {
    fprintf(stderr, "Internal or IO error.\n");
    return 2;
  }
}
