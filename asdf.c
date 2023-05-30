#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct FileSet{
  int size;
  char *content;
  struct FileSet *next;
}FileSet;

FileSet *head = NULL;
FileSet *current = NULL;

void newPath(char *dest, char *src1, char *src2) {
  strcpy(dest, src1);
  strcat(dest, "/");
  strcat(dest, src2);
}


void linkFiles(char *void_name, FileSet *file){
  file = head;
  char * dir_name = (char*)void_name;
  DIR * dir = opendir(dir_name);
  if (dir == NULL) {
    printf("Cannot open dir.\n");
    exit(1);
  }

  current = head;

  struct dirent *directory_entity;
  while ((directory_entity = readdir(dir)) != NULL) {
    if (strcmp(directory_entity->d_name, ".") == 0 || strcmp(directory_entity->d_name, "..") == 0) continue;
    struct stat fileState;
    char dir_path[256];

    newPath(dir_path, dir_name, directory_entity->d_name);

    if (lstat(dir_path, &fileState) != 0) printf("lstat error\n");
    
    else {
      //  Recursive call
      if (S_ISDIR(fileState.st_mode)) {
        linkFiles(dir_path,file); 
      }
      else if (S_ISREG(fileState.st_mode)){
        FileSet *node = (FileSet *)malloc(sizeof(FileSet));
        node->size = fileState.st_size;

        FILE *f = fopen(dir_path, "r");
        if(f == NULL){
          printf("No file found");
          exit(1);
        }
        char *buffer1 = (char*)malloc(fileState.st_size);
        fread(buffer1, fileState.st_size, 1, f);
        strcpy(node->content, buffer1);
        fclose(f);

        node->next = NULL;

        if(head == NULL){
          node = head;
          current = head;
        }else{
          current->next = node;
          current = node;
        }
      }
    }
  }

  closedir(dir);
}


int main()
{
  const char *path = "/Users/Jeongwon";
  FileSet *file;
  linkFiles(path, file);
  return 0;

}