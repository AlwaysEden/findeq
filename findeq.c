/*
  1. you can take the string of command line arguments. - clear
  2. you can acess the files and directories to find the objective.
*/
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>


void traverseDirectory(char *path) {
    DIR *dir;
    struct dirent * entry;
    struct stat fileState;

    dir = opendir(path);
    if(dir == NULL){
      printf("Couldn't open directory");
      exit(1);
    }


    if (lstat(path, &fileState) != 0) printf("lstat error\n");
    
    else {
      //  Recursive call
      if (S_ISDIR(fileState.st_mode)) traverseDirectory(path); 
      else if (S_ISREG(fileState.st_mode)) printf("%s\n", path);
    }

    int standardFile = 0;
    char* buffer1;

    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_DIR) { 
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
          char newPath[1024];
          snprintf(newPath, sizeof(newPath), "%s/%s", path, entry->d_name);
          traverseDirectory(newPath);
        }
      } else if(entry->d_type == DT_REG){
        char file2_path[1024];
        snprintf(file2_path, sizeof(file2_path), "%s/%s", path, entry->d_name);

        FILE *file2 = fopen((file2_path), "r");
        fseek(file2, 0, SEEK_END);
        int size2 = ftell(file2);
        rewind(file2);
        char* buffer2 = (char*)malloc(size2);
        fread(buffer2, size2, 1, file2);

        
        int result = memcmp(buffer1, buffer2, size2);
        fclose(file2);
        if(result == 0) printf("%s/%s\n", path, entry->d_name);
        result = 0;
      }
    }
  closedir(dir);
}
//When comparing the files, I need to be careful about how they are compared. I think that I can't use the name and link as the comparing standards. Therefore, you have to read again the assignment file.
int main(int argc, char **argv) {
    char *option = argv[1];
    char *path = "./Users/jeongwon/Desktop/DB28";
    if(path[0] == '.') path = path + 1;
    traverseDirectory(path);
    return 0;
}

