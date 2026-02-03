#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    int fd;
    char *addr;
    size_t length = 4096 * 4; // 4 pages
    char *vec;
    long page_size = sysconf(_SC_PAGESIZE);

    // 1. Create a temporary file and map it
    fd = open("testfile", O_RDWR | O_CREAT | O_TRUNC, 0666);
    ftruncate(fd, length);
    addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // 2. Prepare vector array for mincore (1 byte per page)
    vec = calloc(1, (length + page_size - 1) / page_size);

    // 3. Check residency before touching memory
    mincore(addr, length, vec);
    printf("Before touching:\n");
    for (int i = 0; i < (length + page_size - 1) / page_size; i++) {
        printf("Page %d: %s\n", i, (vec[i] & 1) ? "Resident" : "Not Resident");
    }

    // 4. Touch the memory to bring it into core
    addr[0] = 'a';
    addr[page_size * 2] = 'b';

    // 5. Check residency again
    mincore(addr, length, vec);
    printf("\nAfter touching pages 0 and 2:\n");
    for (int i = 0; i < (length + page_size - 1) / page_size; i++) {
        printf("Page %d: %s\n", i, (vec[i] & 1) ? "Resident" : "Not Resident");
    }

    // Cleanup
    munmap(addr, length);
    close(fd);
    free(vec);
    unlink("testfile");

    return 0;
}

