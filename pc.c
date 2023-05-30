#include <stdio.h>
#include <dirent.h>
#include <string.h>


void traverseDirectory(const char *path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Fail to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) { //
                char newPath[1024];
                snprintf(newPath, sizeof(newPath), "%s/%s", path, entry->d_name);
                traverseDirectory(newPath);
            }
        } else {
            printf("%s/%s\n", path, entry->d_name);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    
    const char *path = "/Users/Jeongwon";
    traverseDirectory(path);
    return 0;
}

