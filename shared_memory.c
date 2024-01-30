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

// ----------------------------------------------------------------------------
// sm_create
// ----------------------------------------------------------------------------
void sm_create(char *sm_name, char *size_str) {
    int size = atoi(size_str);
    if (size <= 0) {
        printf("Invalid size provided.\n");
        exit(1);
    }
    int fd = shm_open(sm_name, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(1);
    }
    printf("Shared memory '%s' created with size %d.\n",sm_name,size);
    // On Linux, found at /dev/shm, on Mac OS you may call 'ipcs -m'
}

// ----------------------------------------------------------------------------
// sm_write
// ----------------------------------------------------------------------------
void sm_write(char *name, char *offset_str, char *value_str) {
    int offset = atoi(offset_str);
    int value = atoi(value_str);
    int fd = shm_open(name, O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(1);
    }
    if (offset < 0 || offset >= sb.st_size) {
        printf("Invalid offset.\n");
        exit(1);
    }
    char *ptr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    memcpy(ptr+offset,&value,sizeof(int));
    printf("Value '%d' written at offset '%d' in shared memory '%s'.\n", value, offset, name);
}

// ----------------------------------------------------------------------------
// sm_read
// ----------------------------------------------------------------------------
void sm_read(char *name, char *offset_str) {
    int offset = atoi(offset_str);
    int fd = shm_open(name, O_RDONLY, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(1);
    }
    if (offset < 0 || offset >= sb.st_size) {
        printf("Invalid offset.\n");
        exit(1);
    }
    char *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    int value = 0;
    memcpy(&value,ptr+offset,sizeof(int));
    printf("Value at offset '%d' in shared memory '%s': %d\n", offset, name, value);
}

// ----------------------------------------------------------------------------
// sm_list
// ----------------------------------------------------------------------------
void hex_dump(char *ptr, int length) {
    if (length > 256) {
        length = 256;
    }
    for (int i = 0; i < length; i += 16) {
        printf("0x%16p: ", ptr + i);
        for (int j = i; j < i + 16 && j < length; ++j) {
            printf("%02x ", (unsigned char)ptr[j]);
        }
        printf("\n");
    }
}

void sm_list(char *sm_name) {
    int fd = shm_open(sm_name, O_RDONLY, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(1);
    }
    char *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    hex_dump(ptr, sb.st_size);
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
        if (argc < 4) {
            printf("No name and/or size provided for 'create' command.\n");
            return 1;
        }
        sm_create(argv[2],argv[3]);
    } else if (strcmp(argv[1], "read") == 0) {
        // Implement "read" command
        if (argc < 4) {
            printf("No name and/or offset provided for 'read' command.\n");
            return 1;
        }
        sm_read(argv[2], argv[3]);
    } else if (strcmp(argv[1], "write") == 0) {
        if (argc < 5) {
            printf("No name, offset, and/or value provided for 'write' command.\n");
            return 1;
        }
        sm_write(argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "destroy") == 0) {
        if (argc < 3) {
            printf("No name provided for 'destroy' command.\n");
            return 1;
        }
        sm_destroy(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        if (argc < 3) {
            printf("No name provided for 'list' command.\n");
            return 1;
        }
        sm_list(argv[2]);
    } else {
        printf("Invalid command. Please use 'create', 'read', 'write' or 'destroy'\n");
        return 1;
    }

    return 0;
}