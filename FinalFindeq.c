#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

pthread_mutex_t lock;
off_t wastedSize = 0;
int numOfDuplicatedFile = 0;
FILE *outputPath;
char isFile[20];

typedef struct FileList {
  int number;
  off_t fileSize;
   char file_directory[256];
   struct FileList * next;
  struct FileList * dupFile;
}file_list;

file_list * global_header = NULL;

typedef struct MultiArg {
  struct FileList * file_list_header;
  int numOfThread;
  int num;
  size_t minSize;
}multi_arg;

void newPath(char *dest, char *src1, char *src2) {
  strcpy(dest, src1);
  strcat(dest, "/");
  strcat(dest, src2);
}

void * travelDir(char * dir_name) {
  DIR * dir = opendir(dir_name);
  if (dir == NULL) {
    printf("Cannot open dir.\n");
    exit(1);
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
      else if (S_ISREG(fileState.st_mode)) {
        int count = 0;
        file_list * temp = global_header;
        while (temp->next != NULL) {
          temp = temp->next;
          count++;
        }
        file_list * data = (file_list*)malloc(sizeof(file_list));
        data->number = ++count;
        data->fileSize = fileState.st_size;
        strcpy(data->file_directory, dir_path);
        data->next = NULL;
        data->dupFile = NULL;
        temp->next = data;
      }
    }
  }

  closedir(dir);
  return NULL;
}

int compareFiles(const char* file1Path, const char* file2Path) {
  FILE* file1 = fopen(file1Path, "rb");
  FILE* file2 = fopen(file2Path, "rb");

  if (file1 == NULL || file2 == NULL) {
    printf("unable to open a file\n");
    return -1;
  }

  int equal = 1;
  char ch1 = getc(file1);
  char ch2 = getc(file2);
  
  while (ch1 != EOF && ch2 != EOF) {
    if (ch1 != ch2) {
      equal = 0;
      break;
    }

    ch1 = getc(file1);
    ch2 = getc(file2);
  }

  if (ch1 != ch2) {
    equal = 0;
  }

  fclose(file1);
  fclose(file2);

  return equal;
}

void * findDuple(void* mul_arg) {
  multi_arg * mul_arg_func = (multi_arg*)mul_arg;
  file_list * temp, * temp2, * temp3;
  int i, target = mul_arg_func->num;
  struct stat fileState;

  while(1) {
    for (i = 0, temp = mul_arg_func->file_list_header->next; i < target && (temp->next != NULL); i++) {
      temp = temp->next;
    }

    //printf("! %s\n", temp->file_directory);
    if (lstat(temp->file_directory, &fileState) != 0) printf("lstat error\n");

    if (i == target && fileState.st_size > mul_arg_func->minSize) {
      //  numOfThread > numOfFounded file
      temp2 = temp;
      temp3 = temp2->next;
      while (temp3 != NULL) {
        int result = compareFiles(temp2->file_directory, temp3->file_directory);
        if (result == 1) {
          //printf("Same [[%s]] and [[%s]]\n", temp2->file_directory, temp3->file_directory);
          
          pthread_mutex_lock(&lock);
          temp2->dupFile = temp3;
          temp2 = temp3;
          pthread_mutex_unlock(&lock);
        } 
        temp3 = temp3->next;
      }
    }

    int count = 0;
    temp2 = temp;
    while (temp2 != NULL && count < mul_arg_func->numOfThread) {
      temp2 = temp2->next;
      count++;
    }

    if (temp2 != NULL) {
      target = temp2->number - 1;
    }
    
    else {
      break;
    }
  }

  return NULL;
}

void calculateGlobal() {
  file_list * temp2, * temp;
  struct stat fileState;
  wastedSize = 0;
  numOfDuplicatedFile = 0;

  for (temp = global_header->next; temp != NULL; temp = temp->next) {
    temp2 = temp;
    lstat(temp2->file_directory, &fileState);
    if (temp2->dupFile != NULL) {
      numOfDuplicatedFile++;
      while (temp2 != NULL) {
        wastedSize += fileState.st_size;
        temp2 = temp2->dupFile;
      }
    }
  }
}

void PrintResult() {
  file_list * temp2, * temp;

  printf("\nWasted space size: %ld\n", wastedSize);
  printf("Number of category that duplicated file: %d\n", numOfDuplicatedFile);
  printf("[\n");
  for (temp = global_header->next; temp != NULL; temp = temp->next) {
    temp2 = temp;
    if (temp2->dupFile != NULL) {
      printf("  [\n");
      while (temp2 != NULL) {
        printf("    %s", temp2->file_directory);
        temp2 = temp2->dupFile;
        if (temp2 != NULL) printf(",");
        printf("\n");
      }
      printf("  ],\n");
    }
  }
  printf("]\n");
}

void FPrintResult() {
  file_list * temp2, * temp;

  fprintf(outputPath, "\nWasted space size: %ld\n", wastedSize);
  fprintf(outputPath, "Number of category that duplicated file: %d\n", numOfDuplicatedFile);
  fprintf(outputPath, "[\n");
  for (temp = global_header->next; temp != NULL; temp = temp->next) {
    temp2 = temp;
    if (temp2->dupFile != NULL) {
      fprintf(outputPath, "  [\n");
      while (temp2 != NULL) {
        fprintf(outputPath, "    %s\n", temp2->file_directory);
        temp2 = temp2->dupFile;
      }
      fprintf(outputPath, "  ],\n");
    }
  }
  fprintf(outputPath, "]\n");

  if (strcmp(isFile, "-o=FILE") == 0) {
    fclose(outputPath);
    outputPath = fopen("FileOutput.txt", "a+");
  }
}

void alarmHandler(int signum) {
  calculateGlobal();
  FPrintResult();
  alarm(5);
}

void sigHandler(int signum) {
  calculateGlobal();
  PrintResult();
  exit(1);
}

int main(int argc, char *argv[]) {
  int numOfThread = 1;
  size_t minSize = 1024;
  outputPath = fdopen(2,"a+");
  signal(SIGALRM, alarmHandler);
  signal(SIGINT, sigHandler);
  alarm(5);
  
  if (argc < 2) {
    printf("No directory error \n");
    return 0;
  }

  for (int i = 0; i < argc; i++) {
    if (strncmp(argv[i], "-t=", 3) == 0) {
      numOfThread = atoi(argv[i] + 3);
      if (numOfThread == 0) {
        printf("-t wrong input\n");
        return 0;
      }

      if (numOfThread > 64) {
        printf("thread no more than 64.\n");
        return 0;
      }
    }

    else if (strncmp(argv[i], "-m=", 3) == 0) {
      minSize = atoi(argv[i] + 3);
      if (minSize == 0) {
        printf("-m wrong input\n");
        return 0;
      }
    }

    else if (strncmp(argv[i], "-o=", 3) == 0) {
      if(strcmp(argv[i], "-o=FILE") == 0){
        fclose(outputPath);
        strcpy(isFile, argv[i]);
        outputPath = fopen("FileOutput.txt", "a+");
      }

      else {
        printf("-o wrong input\n");
        return 0;
      }
    }
  }

  //printf("[[%d]] and [[%ld]]\n", numOfThread, minSize);

  if (argv[argc - 1][strlen(argv[argc - 1]) - 1] == '/') argv[argc - 1][strlen(argv[argc - 1]) - 1] = '\0';

  file_list * header = (file_list*)malloc(sizeof(file_list));
  header->number = 0;
  strcpy(header->file_directory, " ");
  header->next = NULL;
  header->dupFile = NULL;
  global_header = header;

  travelDir(argv[argc - 1]);

  // file_list * temp;
  // for (temp = header->next; temp != NULL; temp = temp->next) {
  //   printf("%d and %s\n", temp->number, temp->file_directory);
  // }

  pthread_t threads[numOfThread];
  pthread_mutex_init(&lock,NULL);

  for (int i = 0; i < numOfThread; i++) {
    multi_arg * mul_arg = (multi_arg*)malloc(sizeof(multi_arg));
    mul_arg->file_list_header = header;
    mul_arg->numOfThread = numOfThread;
    mul_arg->num = i;
    mul_arg->minSize = minSize;
    pthread_create(&(threads[i]), NULL, findDuple, (void*)mul_arg);
  }

  for (int i = 0; i < numOfThread; i++) {
    pthread_join(threads[i],NULL);
  }

  while (1) {}

  //PrintResult();
  
  return 0;
}

//findeq.c -t= ~ -m= ~ -o= ~ -o findeq -lpthread
// 1-7 2-3 4-5 같음