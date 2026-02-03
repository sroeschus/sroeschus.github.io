/*
* uname -a
* FreeBSD vmsrv 15.0-RELEASE-p1 FreeBSD 15.0-RELEASE-p1 releng/15.0-63a5b645b23e GENERIC amd64
*
* sysctl vm.pmap.pg_ps_enabled
* vm.pmap.pg_ps_enabled: 1
*
* Build executable:
* clang -Wall -pedantic -o test_large_pages ./test_large_pages.c
*
* Run executable:
* ./test_large_pages
* if (4096 == 2097152)
* if (2097152 == 2097152)
* shm_create_largepage()
* ftruncate()
* mmap()
* pause()
*
* Display number of pages allocated:
* procstat vm $(pgrep test_large_pages)
* ...
*   PID              START                END PRT     RES    PRES REF SHD FLAG  TP PATH
* 38528        0x822e00000        0x922e00000 rw- 1048576 1048576   2   0 ----- ph  posixshm@anon
*
* Number of pages allocated is way more than the expected 2048
*/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define PGSIZE_ARRAY_LEN 16U

int main(void)
{
	size_t pgsize_array[PGSIZE_ARRAY_LEN]; /* Array of page sizes */
	int    pgsize_array_cnt;               /* Valid array elements count */
	int    pgsize_array_idx = -1;          /* Page size index */

	int    fd;
	int    int_val;

	/* Use 2 MiB page sizes */
	const size_t pgsize = 2097152U;

	/*
	*  Allocate 4 GiB shared memory segment. This should require:
	*  4 GiB / 2 MiB = 2048 pages
	*/
	const size_t num_pages = 32;
	// const size_t shm_size  = UINT64_C(1024 * 1024) * num_pages;
	const size_t shm_size  = pgsize * num_pages;
	void        *shm_ptr;

	/* Get all supported page sizes */
	pgsize_array_cnt = getpagesizes(pgsize_array, PGSIZE_ARRAY_LEN);
	assert(pgsize_array_cnt >= 0);

	/*
	* For the required page size shm_create_largepage() expects this to
	* be specified as an index into the array as filled by getpagesizes().
	* See shm_open(2) for more details.
	*/
	for (int i = 0; i < pgsize_array_cnt; i++)
	{
		/* Locate the array index which matches specified page size */
		printf("if (%zu == %zu)\n", pgsize_array[i], pgsize);
		if (pgsize_array[i] == pgsize)
		{
			pgsize_array_idx = i;
			// break;
		}
	}
	assert(pgsize_array_idx >= 0);

	/* Open anonymous shared memory object */
	printf("shm_create_largepage()\n");
	fd = shm_create_largepage(SHM_ANON,
                              O_RDWR | O_CREAT,
                              pgsize_array_idx,
		                      SHM_LARGEPAGE_ALLOC_DEFAULT,
                              0);
	assert(fd >= 0);

	/* Allocate pages */
	printf("ftruncate()\n");
	int_val = ftruncate(fd, (off_t)shm_size);
	assert(int_val == 0);

	/* Map pages */
	printf("mmap()\n");
	shm_ptr = mmap(NULL,
                   shm_size,
                   PROT_READ | PROT_WRITE,
		           MAP_SHARED | MAP_NOCORE,
                   fd,
                   0);
	assert(shm_ptr != MAP_FAILED);

	memset(shm_ptr, 0, 32 * 1024 * 1024);

    /* Use mincore to check if pages are resident */
    char *vec = malloc(num_pages);
    if (mincore(shm_ptr, shm_size, vec) == -1) {
        perror("mincore");
    } else {
        for (size_t i = 0; i < num_pages; i++) {
            // Check if page is resident and if it's a large page
            if (vec[i] & MINCORE_INCORE) {
                printf("Page %zu: Resident\n", i);
                if (vec[i] & MINCORE_PSIND(pgsize_array_idx)) {
                    printf("  -> Large Page (PSIND: %d)\n", pgsize_array_idx);
                }
            } else {
                printf("Page %zu: Not resident\n", i);
            }
        }
    }

	printf("pause()\n");
	pause();
}
