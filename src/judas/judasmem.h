/*
 * Internal header file: memory locking
 */
int judas_memlock(void *start, unsigned size);
int judas_memunlock(void *start, unsigned size);
void *locked_malloc(int size);
void locked_free(void *address);
