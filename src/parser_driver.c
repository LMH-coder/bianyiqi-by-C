#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser_driver.h"

// Bison / scanner interface
int yyparse(void *scanner, ParserDriver *drv);
int yylex_init_extra(ParserDriver *drv, void **scanner);
int yylex_destroy(void *scanner);
int yy_scan_bytes(const char *bytes, size_t len, void *scanner);
void yyset_in(FILE *in_str, void *scanner);

// parse a buffer
int parse_buffer(ParserDriver *drv) {
  void *scanner = NULL;
  if (yylex_init_extra(drv, &scanner) != 0) return 2;

  // Our re2c scanner uses drv->buf directly; these stubs keep API symmetric.
  yy_scan_bytes(drv->buf, drv->len, scanner);

  int rc = yyparse(scanner, drv);

  yylex_destroy(scanner);
  if (rc == 0) return 0; // OK

  if (drv->err_msg[0] != '\0') {
    fprintf(stderr, "Syntax error at %s:%d:%d: %s\n",
            drv->filename ? drv->filename : "<stdin>",
            drv->err_line, drv->err_col, drv->err_msg);
  } else {
    fprintf(stderr, "Syntax error at %s:%d:%d\n",
            drv->filename ? drv->filename : "<stdin>", drv->err_line, drv->err_col);
  }
  return 1;
}

// parse a file
int parse_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    perror("fopen");
    return 2;
  }
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (sz < 0) { fclose(f); return 2; }

  char *buf = (char*)malloc((size_t)sz + 2);
  if (!buf) { fclose(f); return 2; }

  size_t n = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  buf[n] = '\n'; // sentinel for ASI at EOF
  buf[n+1] = '\0';

  ParserDriver drv;
  memset(&drv, 0, sizeof(drv));
  drv.filename = path;
  drv.buf = buf;
  drv.len = n + 1;
  drv.pos = 0;
  drv.line = 1;
  drv.col = 1;
  drv.err_line = 1;
  drv.err_col = 1;
  drv.has_line_terminator = 0;

  int rc = parse_buffer(&drv);
  free(buf);
  return rc;
}
