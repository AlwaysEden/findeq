#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
  char * a = "aslkdfjalsdkjfds";
  char * b = "";
  strcpy(b,a);

  printf("%s\n",b);
}