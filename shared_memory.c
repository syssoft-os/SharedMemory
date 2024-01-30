#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>           
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void to_lower(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower((unsigned char)str[i]);
    }
}

char* addPidSuffix(const char* str) {
    pid_t pid = getpid();
    int strLength = strlen(str);
    int pidLength = snprintf(NULL, 0, "_%d", pid);
    char* result = malloc(strLength + pidLength + 1);
    if (result == NULL) {
        perror("Error allocating memory");
        exit(1);
    }
    strcpy(result, str);
    sprintf(result + strLength, "_%d", pid);

    return result;
}

// ----------------------------------------------------------------------------
// sm_create
// ----------------------------------------------------------------------------
void sm_create(char *size_str) {
    int size = atoi(size_str);
    if (size <= 0) {
        printf("Invalid size provided.\n");
    }
    char* sm_name = addPidSuffix("shared_memory");
    int fd = shm_open(sm_name, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
    }

    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
    }

    printf("Shared memory '%s' created with size %d.\n",sm_name,size);
    // On Linux, found at /dev/shm, on Mac OS you may call 'ipcs -m'
    free(sm_name);
}

// ----------------------------------------------------------------------------
// sm_destroy
// ----------------------------------------------------------------------------
void sm_destroy(char *sm_name) {
    if (shm_unlink(sm_name) == -1) {
        perror("shm_unlink");
        exit(1);
    }
    printf("Shared memory '%s' destroyed.\n", sm_name);
}

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No command provided.\n");
        return 1;
    }

    to_lower(argv[1]);

    if (strcmp(argv[1], "create") == 0) {
        if (argc < 3) {
            printf("No size provided for 'create' command.\n");
            return 1;
        }
        sm_create(argv[2]);

    } else if (strcmp(argv[1], "read") == 0) {
        // Implement "read" command
    } else if (strcmp(argv[1], "write") == 0) {
        // Implement "write" command
    } else if (strcmp(argv[1], "destroy") == 0) {
        if (argc < 3) {
            printf("No name provided for 'destroy' command.\n");
            return 1;
        }
    sm_destroy(argv[2]);
    } else {
        printf("Invalid command. Please use 'create', 'read', 'write' or 'destroy'\n");
        return 1;
    }

    return 0;
}