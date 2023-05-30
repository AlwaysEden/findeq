#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exploreDirectory(const char* path) {
    FILE* file;
    char command[256];
    char filename[256];
    
    sprintf(command, "ls -a %s", path);  // Command to list files in the directory
    
    file = popen(command, "r");  // Open a pipe to execute the command and read the output
    
    if (file == NULL) {
        printf("Error opening pipe.\n");
        return;
    }
    
    while (fgets(filename, sizeof(filename), file) != NULL) {

      int a = (int)strlen(filename);
      int b = (int)strlen(path);
      char currentPath[1024];
      snprintf(currentPath, sizeof(filename), "%s/%s", path, filename);

      FILE *buffer = fopen(currentPath, "r");
      if(buffer == NULL) { //디렉토리라는 의미
        if (strncmp(filename, ".",1) != 0){
          char file_path[strlen(path) + strlen(filename)];
          snprintf(file_path, sizeof(file_path), "%s/%s", currentPath, filename);
          exploreDirectory(file_path);
        }
      }else{
        printf("%s", filename);  // Print each file name
      }
    }
    
    pclose(file);  // Close the pipe
    
    printf("\n");
}

struct(
  int size;
  char *content[3];
)
구조체 배열
int main() {
    char *path = "./Users/jeongwon/Desktop/DB28";
    if(path[0] == '.') path = path + 1;
    exploreDirectory(path);  // Pass the directory path you want to explore
    
    return 0;
}
