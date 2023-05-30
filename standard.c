#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

pthread_mutex_t lock;

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



void * travelDir(void *void_name) {
  char * dir_name = (char*)void_name;
  DIR * dir = opendir(dir_name);
  if (dir == NULL) {
    printf("Cannot open dir.\n");
    return NULL;
  }

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
        travelDir(dir_path); 
      }
      else if (S_ISREG(fileState.st_mode)) printf("%s\n", dir_path);
    }
  }

  closedir(dir);
  return NULL;
}

void linkFiles(void *void_name){
  char * dir_name = (char*)void_name;
  DIR * dir = opendir(dir_name);
  if (dir == NULL) {
    printf("Cannot open dir.\n");
    return;
  }

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
        linkFiles(dir_path); 
      }
      else if (S_ISREG(fileState.st_mode)){
        FileSet *node = (FileSet *)malloc(sizeof(FileSet));
        node->size = fileState.st_size;

        FILE *f = fopen(dir_path, "r");
        if(f == NULL){
          printf("No file found");
          exit(1);
        }
        char *buffer1 = (char*)malloc((int)fileState.st_size);
        node->content = (char*)malloc((int)fileState.st_size);
        fread(buffer1, (int)fileState.st_size, (int)fileState.st_size, f);
        strcpy(node->content, buffer1);
        fclose(f);

        node->next = NULL;

        if(head == NULL){
          head = node;
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

int main(int argc, char *argv[]) {
  // int numOfThread = 1, numOfSize = 1024;
  // if (argc < 2) {
  //   printf("No directory error \n");
  //   return 0;
  // }

  // for (int i = 0; i < argc; i++) {
  //   if (strncmp(argv[i], "-t=", 3) == 0) {
  //     char * value;
  //     for (int i = 3; i < strlen(argv[i]); i++) {
        

  //     }
  //     numOfThread = atoi(argv[i]);
  //     if (numOfThread == 0) {
  //       printf("-t wrong input\n");
  //       return 0;
  //     }
  //   }

  //   else if (strncmp(argv[i], "-m=", 3) == 0) {
  //     numOfSize = atoi(argv[i + 1]);
  //     if (numOfSize == 0) {
  //       printf("-m wrong input\n");
  //       return 0;
  //     }
  //   }

  //   else if (strncmp(argv[i], "-o=", 3) == 0) {

  //   }
  // }

  // printf("[[%d]] and [[%d]]\n", numOfThread, numOfSize);

  // if (argv[1][strlen(argv[1]) - 1] == '/') argv[1][strlen(argv[1]) - 1] = '\0';

  // travelDir((void *)argv[argc-1]);
  // travelDir("/Users/jeongwon/Desktop/");
  current = head;
  linkFiles("/Users/jeongwon/Desktop/");
  current = head;
  int p = 0;
  while(1){
    printf("%d\n", current->size);
    printf("%s\n", current->content);
    current = current->next;
    p++;
    if(p == 100) break;
  }
  return 0;
}

//findeq.c -t ~ -m ~ -o ~ -o findeq -lpthread