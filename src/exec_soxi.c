#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void finfo(char *str){
  int res;
  char cmd[109];
  strcpy(cmd, "soxi -T ");
  strncat(cmd, str, 100);
  if ((res = system(cmd)) == -1)
    return;
}

int main(int argc, char*argv[]) {
  finfo(argv[1]);
  return EXIT_SUCCESS;
}
