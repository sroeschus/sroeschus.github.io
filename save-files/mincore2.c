#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    const char *shm_name = "/test_largepage";
    size_t page_size = 2 * 1024 * 1024; // Example: 2MB page
    size_t num_pages = 4;
    size_t total_size = page_size * num_pages;
    int psind = 1; // Index into getpagesizes(3), typically 2MB is index 1

    // 1. Create a large page shared memory object
    int fd = shm_create_largepage(SHM_ANON, O_CREAT | O_RDWR, psind, SHM_LARGEPAGE_ALLOC_DEFAULT, 0);
    if (fd == -1) {
        perror("shm_create_largepage");
        exit(1);
    }

    // 2. Set the size
    if (ftruncate(fd, total_size) == -1) {
        perror("ftruncate");
        shm_unlink(shm_name);
        exit(1);
    }

    // 3. Map the memory
    void *addr = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        shm_unlink(shm_name);
        exit(1);
    }

    // 4. Initialize memory to force physical allocation
    memset(addr, 0, total_size);

    // 5. Use mincore to check if pages are resident
    char *vec = malloc(num_pages);
    if (mincore(addr, total_size, vec) == -1) {
        perror("mincore");
    } else {
        for (size_t i = 0; i < num_pages; i++) {
            // Check if page is resident and if it's a large page
            if (vec[i] & MINCORE_INCORE) {
                printf("Page %zu: Resident\n", i);
                if (vec[i] & MINCORE_PSIND(psind)) {
                    printf("  -> Large Page (PSIND: %d)\n", psind);
                }
            } else {
                printf("Page %zu: Not resident\n", i);
            }
        }
    }

    // Cleanup
    free(vec);
    munmap(addr, total_size);
    close(fd);
    shm_unlink(shm_name);

    return 0;
}

