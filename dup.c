#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(){
    FILE *outputPath;

    while(1){
      outputPath = fopen("file.txt","a+");
      fprintf(outputPath,"Hello\n");
      fprintf(outputPath,"Hello\n");
      sleep(5);
      fclose(outputPath);
  }
  return 0;
}