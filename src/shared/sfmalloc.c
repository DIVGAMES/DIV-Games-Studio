/*
 * MIT License
 *
 * Copyright (c) 2020 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Undef SF_TRANSPARENT to ourselves. */
#undef SF_TRANSPARENT

#include "sfmalloc.h"

/* User signal handlers. */
#ifndef SF_DISABLE_SIGNAL_HANDLERS
static void (*sigint)(int) = NULL;
static void (*sigterm)(int) = NULL;
#endif

/*
 * SafeMalloc main context structure.
 */
static struct safemalloc sf = {
	0, 0, 0, 0, 0, 0,
#ifndef SFMALLOC_SAVE_SPACE
	0, 0,
#endif
	0, 0, 0, 0
};

/*
 * Safe Malloc mutex.
 */
static pthread_mutex_t sf_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Forward declarations. */
static int hashtable_init(struct hashtable **ht);
static int hashtable_finish(struct hashtable **ht);
static int hashtable_add(struct hashtable **ht, struct address *key);
static void* hashtable_remove(struct hashtable **ht, uintptr_t address);
static void* hashtable_get(struct hashtable **ht, uintptr_t key);
#if HASHTABLE_ENABLE_STATS == 1
static void hashtable_print_stats(struct hashtable **ht);
#endif

/* Hash stuffs */
static int hashtable_cmp_ptr(uintptr_t key1, uintptr_t key2);
static uint64_t hashtable_splitmix64_hash(uintptr_t key, size_t size);

/**
 * Deallocates all the memory at the process termination, warns
 * the user for possible memory leaks (if VERBOSE_MODE_2) and
 * shows the data gathere at the moment.
 */
static void sf_exit(void)
{
pthread_mutex_lock(&sf_mutex);
	sf.exiting = 1;
	if (sf.verbose_mode == VERBOSE_MODE_3)
	{
		fprintf(stderr,
			"\n=====================================================================\n");
		fprintf(stderr, "SafeMalloc v0.1 - Theldus, verbose mode\n");
		fprintf(stderr, "---------------------------------------------\n\n");
	}
pthread_mutex_unlock(&sf_mutex);
	sf_free_all();
pthread_mutex_lock(&sf_mutex);
	if (sf.verbose_mode == VERBOSE_MODE_3)
	{
		fprintf(stderr, "---------------------\n");
		fprintf(stderr, "     Usage data:\n");
		fprintf(stderr, "---------------------\n\n");
		fprintf(stderr, "Allocation infos:\n");
		fprintf(stderr, "-----------------\n");
		fprintf(stderr, "Total allocated (bytes): %zu\n", sf.total_allocated);
#ifndef SFMALLOC_SAVE_SPACE
		fprintf(stderr, "Peak memory (bytes): %zu\n", sf.peak_memory);
		fprintf(stderr, "Leaked (bytes): %zu\n\n", sf.current_memory);
#endif
		fprintf(stderr, "Calls info:\n");
		fprintf(stderr, "-----------\n");
		fprintf(stderr, "Malloc calls:  %zu\n", sf.malloc_calls);
		fprintf(stderr, "Calloc calls:  %zu\n", sf.calloc_calls);
		fprintf(stderr, "Realloc calls: %zu\n", sf.realloc_calls);
		fprintf(stderr, "Free calls:    %zu\n", sf.free_calls);
		fprintf(stderr,
			"=====================================================================\n");
	}
pthread_mutex_unlock(&sf_mutex);
	pthread_mutex_destroy(&sf_mutex);
}

#ifndef SF_DISABLE_SIGNAL_HANDLERS
/**
 * Register a SIGINT.
 */
void sf_reg_sigint(void (*handler)(int))
{
	sigint = handler;
}

/**
 * Register a SIGTERM.
 */
void sf_reg_sigterm(void (*handler)(int))
{
	sigterm = handler;
}

/**
 * SIGINT and SIGTERM handler that calls sf_exit if the process
 * get interrupted.
 *
 * @param s Signal number.
 */
static void sf_sig_handler(int s)
{
	sf_exit();
	if (sigint != NULL && s == SIGINT)
		sigint(s);
	if (sigterm != NULL && s == SIGTERM)
		sigterm(s);
	_exit(0);
}
#endif

/**
 * Deallocates all the memory allocated at the moment and warns
 * the user (if VERBOSE_MODE_2) for possible leaks.
 */
void sf_free_all(void)
{
	struct hashtable *h;   /* Hashtable.    */
	struct address *l_ptr; /* List pointer. */
	size_t i;              /* Loop index.   */

pthread_mutex_lock(&sf_mutex);
	h = sf.ht;

	/* Invalid array. */
	if (h == NULL)
	{
		pthread_mutex_unlock(&sf_mutex);
		return;
	}

	if (sf.exiting && sf.verbose_mode & VERBOSE_MODE_2 && h->elements)
	{
		fprintf(stderr,
			"Memory leaks were found during execution, please check below and fix.\n");
		fprintf(stderr,
			"----------------------------------------------------------------------\n");
	}

	/*
	 * For each bucket, deallocates each list.
	 */
	for (i = 0; i < h->capacity; i++)
	{
		struct address *l_ptr_next;

		l_ptr = h->bucket[i];
		while (l_ptr != NULL)
		{
			l_ptr_next = l_ptr->next;

			/* If not supressing messages. */
			if (sf.exiting && sf.verbose_mode & VERBOSE_MODE_2)
			{
#ifndef SFMALLOC_SAVE_SPACE
				fprintf(stderr, "- Address 0x%" PRIx64 " (%zu bytes) not freed!\n"
					"  -> Allocated at [%s:%d] %s()\n\n",
					l_ptr->address, l_ptr->bytes, l_ptr->file, l_ptr->line, l_ptr->func);
#else
				fprintf(stderr, "- Address 0x%" PRIx64 " not freed!\n", l_ptr->address);
#endif
			}

			free_fn(l_ptr);
			l_ptr = l_ptr_next;
		}
	}

	free_fn(h->bucket);
	free_fn(h);
	sf.ht = NULL;

	if (!sf.exiting)
	{
		/* Allocate a new hashtable again. */
		if (hashtable_init(&sf.ht) < 0)
		{
			pthread_mutex_unlock(&sf_mutex);
			fprintf(stderr, "sf_free_all: an error has ocurred while allocating"
				" hashtable!\n");
			exit(SFMALLOC_EXIT_FAILURE);
		}
	}

pthread_mutex_unlock(&sf_mutex);
}

/**
 * Initializes the safemalloc context.
 *
 * @param verbose_mode Verbose mode level.
 * @param on_error On Error behaviour.
 *
 * @return Returns 0 if success and a negative
 * number otherwise.
 */
int sf_init(int verbose_mode, int on_error)
{
	// verbose_mode = VERBOSE_MODE_3;
pthread_mutex_lock(&sf_mutex);
	/* Already initialized. */
	if (sf.initialized)
	{
		pthread_mutex_unlock(&sf_mutex);
		return (0);
	}

	/* Try to register our deallocating/leak-check routine. */
	if (atexit(sf_exit))
	{
		fprintf(stderr, "sf_init: failed while trying to register finish"
			" routine!");
		exit(SFMALLOC_EXIT_FAILURE);
	}

#ifndef SF_DISABLE_SIGNAL_HANDLERS
	/* Register our signal handlers. */
	signal(SIGINT, sf_sig_handler);
	signal(SIGTERM, sf_sig_handler);
#endif

	/* Set behaviour. */
	if (verbose_mode < 1 || verbose_mode > 6
		|| on_error  < 1 || on_error > 2)
	{
		fprintf(stderr, "sf_init: invalid value for verbose_mode and/or"
			" on_error\nplease check your values!\n");
		exit(SFMALLOC_EXIT_FAILURE);
	}

	/* Allocate hashtable. */
	if (hashtable_init(&sf.ht) < 0)
	{
		fprintf(stderr, "sf_init: an error has ocurred while allocating"
			" hashtable!\n");
		exit(SFMALLOC_EXIT_FAILURE);
	}

	sf.verbose_mode = verbose_mode;
	sf.on_error     = on_error;
	sf.initialized  = 1;
	sf.exiting      = 0;
pthread_mutex_unlock(&sf_mutex);

	((void)hashtable_finish);
	((void)hashtable_get);
#if HASHTABLE_ENABLE_STATS == 1
	((void)hashtable_print_stats);
#endif
	return (0);
}

/**
 * Allocates @p size bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.
 *
 * If @p size is zero, safemalloc will allocate a single
 * byte, thus, ensuring that no issues should occur with
 * an accidental dereference.
 *
 * @param size Size in bytes to be allocated.
 *
 * @return If success, returns a void pointer pointing to the
 * memory block allocated. On error, the should may return
 * NULL or abort the execution, accordingly with the
 * _on_error_ flag set on sf_init(). ON_ERROR_ABORT is
 * set by default.
 */
void *_sf_malloc(size_t size
#ifndef SFMALLOC_SAVE_SPACE
	,const char *file, const char *func, int line
#endif
)
{
	struct address *addr;

	sf_init(VERBOSE_MODE_2, ON_ERROR_ABORT);

	/* Avoid undefined behaviour. */
	if (!size)
		size = 1;

	/* Try to allocate memory. */
	addr = malloc_fn(sizeof(struct address) + size);
	CHECK_ADDR(addr, return NULL,
		"malloc: failed while allocating %zu bytes!\n", size);

	/* Fill struct addr fields. */
	addr->address = (uintptr_t)(addr+1);
	addr->next    = NULL;

#ifndef SFMALLOC_SAVE_SPACE
	addr->bytes = size;
	addr->file  = file;
	addr->func  = func;
	addr->line  = line;
#endif

pthread_mutex_lock(&sf_mutex);
	/* Fill some infos. */
	sf.total_allocated += size;
	sf.malloc_calls++;
#ifndef SFMALLOC_SAVE_SPACE
	sf.current_memory += size;
	if (sf.current_memory > sf.peak_memory)
		sf.peak_memory = sf.current_memory;
#endif

	/* Add into the addresses list. */
	if (hashtable_add(&sf.ht, addr) < 0)
	{
		/* CHECK_ADDR expects that sf_mutex is unlocked. */
		pthread_mutex_unlock(&sf_mutex);

		free_fn(addr);
		CHECK_ADDR(NULL, return NULL,
			"malloc: failed while adding into the list!\n");
	}
	else
		pthread_mutex_unlock(&sf_mutex);

	/* Return address. */
	return ((void*)(addr+1));
}

/**
 * Allocates memory for an array of @p nmemb elements
 * of @p size bytes each and returns a pointer to the
 * allocated memory. The memory is set to zero.
 *
 * If total amount of bytes is 0 (either nmemb or size
 * equals 0), sfcalloc will allocate a single byte, thus,
 * ensuring that no issues should occur with
 * an accidental dereference.
 *
 * @return If success, returns a void pointer pointing to the
 * memory block allocated. On error, the should may return
 * NULL or abort the execution, accordingly with the
 * _on_error_ flag set on sf_init(). ON_ERROR_ABORT is
 * set by default.
 */
void *_sf_calloc(size_t nmemb, size_t size
#ifndef SFMALLOC_SAVE_SPACE
	,const char *file, const char *func, int line
#endif
)
{
	struct address *addr;
	size_t new_size;

	sf_init(VERBOSE_MODE_2, ON_ERROR_ABORT);

	/*
	 * There are some concerns about @p nmemb and @p size
	 * regarding why calloc have 2 parameters instead of one,
	 * like malloc.
	 *
	 * (See https://stackoverflow.com/q/4083916/3594716)
	 *
	 * Considering this, if nmemb*size exceeds size_t size
	 * the following statement will occur in overflow. Since
	 * I also need to allocate the 'struct address', this wrapper
	 * approach consists of allocating one single block of
	 * size*nmemb + sizeof(struct address) bytes. I'll leave
	 * the overflow check for the calloc_fn.
	 */
	size *= nmemb;

	/* Avoid undefined behaviour. */
	if (!size)
		size = 1;

	new_size = size + (sizeof(struct address));

	/* Try to allocate memory. */
	addr = calloc_fn(1, new_size);
	CHECK_ADDR(addr, return NULL,
		"calloc: failed while allocating %zu bytes!\n", size);

	/* Fill struct addr fields. */
	addr->address = (uintptr_t)(addr+1);
	addr->next    = NULL;

#ifndef SFMALLOC_SAVE_SPACE
	addr->bytes = size;
	addr->file  = file;
	addr->func  = func;
	addr->line  = line;
#endif

pthread_mutex_lock(&sf_mutex);
	/* Fill some infos. */
	sf.total_allocated += size;
	sf.calloc_calls++;
#ifndef SFMALLOC_SAVE_SPACE
	sf.current_memory += size;
	if (sf.current_memory > sf.peak_memory)
		sf.peak_memory = sf.current_memory;
#endif

	/* Add into the addresses list. */
	if (hashtable_add(&sf.ht, addr) < 0)
	{
		/* CHECK_ADDR expects that sf_mutex is unlocked. */
		pthread_mutex_unlock(&sf_mutex);

		free_fn(addr);
		CHECK_ADDR(NULL, return NULL,
			"calloc: failed while adding into the list!\n");
	}
	else
		pthread_mutex_unlock(&sf_mutex);

	/* Return address. */
	return ((void*)(addr+1));
}

/**
 * Changes the size of the memory block pointed to by @p ptr.
 *
 * The function may move the memory block to a new location
 * (whose address is returned by the function). The content
 * of the memory block is preserved up to the lesser of the
 * new and old sizes, even if the block is moved to a new
 * location. If the new @p size is larger, the value of the
 * newly allocated portion is indeterminate.
 *
 * In case that ptr is a NULL pointer, the function behaves
 * like malloc, assigning a new block of size bytes and
 * returning a pointer to its beginning.
 *
 * If @p size is zero, safemalloc will allocate a single
 * byte, thus, ensuring that no issues should occur with
 * an accidental dereference.
 *
 * @param ptr Pointer to be reallocated, or NULL.
 * @param size New size.
 *
 * @return If success, returns a void pointer pointing to the
 * memory block allocated. On error, (if realloc failed or
 * invalid pointer) the should may return
 * NULL or abort the execution, accordingly with the
 * _on_error_ flag set on sf_init(). ON_ERROR_ABORT is
 * set by default.
 */
void *_sf_realloc(void *ptr, size_t size
#ifndef SFMALLOC_SAVE_SPACE
	,const char *file, const char *func, int line
#endif
)
{
	struct address *addr     = NULL;
	struct address *new_addr = NULL;

	sf_init(VERBOSE_MODE_2, ON_ERROR_ABORT);

	/*
	 * Size equals 0 is implementation-defined, in order to avoid
	 * undefined behaviour, safemalloc will allocate a single-byte
	 * memory block that can be freed with sf_free().
	 *
	 * See here: https://stackoverflow.com/q/16759849/3594716
	 * for more infos about.
	 */
	if (!size)
		size = 1;

	/*
	 * Remove from the hashtable, if a valid address.
	 *
	 * The reasoning behind this is simple: the address _may_
	 * be different from the previously allocated, and thus,
	 * the hash too.
	 */
	if (ptr != NULL)
	{
pthread_mutex_lock(&sf_mutex);
		addr = hashtable_remove(&sf.ht, (uintptr_t)ptr);
pthread_mutex_unlock(&sf_mutex);

		CHECK_ADDR(addr, return NULL,
			"sfrealloc: invalid address: %p!\n"
			"make sure that the address was previously allocated by "
			"[m,c,re]alloc!\n", ptr);
	}

	/* Try to allocate memory. */
	new_addr = realloc_fn(addr, sizeof(struct address) + size);

	/*
	 * If ptr != NULL and ptr == NULL, realloc should return with
	 * errors, but should not deallocate the previous address, thus,
	 * we need to insert again in the hashtable, before emiting
	 * errors with CHECK_ADDR =/.
	 */
	if (ptr != NULL && new_addr == NULL)
	{
pthread_mutex_lock(&sf_mutex);
		if (hashtable_add(&sf.ht, addr) < 0)
		{
			/* CHECK_ADDR expects that sf_mutex is unlocked. */
			pthread_mutex_unlock(&sf_mutex);

			free_fn(addr);
			CHECK_ADDR(NULL, return NULL,
				"realloc: failed while re-adding into the list!\n");
		}
		else
			pthread_mutex_unlock(&sf_mutex);
	}
	CHECK_ADDR(new_addr, return NULL,
		"realloc: failed while allocating %zu bytes!\n", size);

	addr = new_addr;

	/* Fill struct addr fields. */
	addr->address = (uintptr_t)(addr+1);
	addr->next    = NULL;

#ifndef SFMALLOC_SAVE_SPACE
	addr->bytes = size;
	addr->file  = file;
	addr->func  = func;
	addr->line  = line;
#endif

pthread_mutex_lock(&sf_mutex);
	/* Fill some infos. */
	sf.total_allocated += size;
	sf.realloc_calls++;
#ifndef SFMALLOC_SAVE_SPACE
	if (sf.current_memory)
		sf.current_memory -= addr->bytes;
	sf.current_memory += size;
	if (sf.current_memory > sf.peak_memory)
		sf.peak_memory = sf.current_memory;
#endif

	/* Add into the addresses list. */
	if (hashtable_add(&sf.ht, addr) < 0)
	{
		/* CHECK_ADDR expects that sf_mutex is unlocked. */
		pthread_mutex_unlock(&sf_mutex);

		free_fn(addr);
		CHECK_ADDR(NULL, return NULL,
			"realloc: failed while adding into the list!\n");
	}
	else
		pthread_mutex_unlock(&sf_mutex);

	/* Return address. */
	return ((void*)(addr+1));
}

/**
 * Frees the memory space pointed to by ptr, which
 * must have been returned by a previous call to
 * sf_malloc(), sf_calloc(), or sfrealloc().
 *
 * If free(ptr) has already been called before and/or,
 * if @p ptr is an invalid pointer sffree may silently
 * returns or abort execution with or without verbose
 * mode, accordingly with the initial flags set on
 * sf_init().
 *
 * If @p ptr is NULL, no operation is performed.
 */
void _sf_free(void *ptr
#ifndef SFMALLOC_SAVE_SPACE
	,const char *file, const char *func, int line
#endif
)
{
	struct address *addr;

	if (ptr == NULL)
		return;

	/* Attempts to remove the previous address. */
pthread_mutex_lock(&sf_mutex);
	addr = hashtable_remove(&sf.ht, (uintptr_t)ptr);
pthread_mutex_unlock(&sf_mutex);

	CHECK_ADDR(addr, return,
		"sffree: invalid address: %p!\nmaybe a double free?\n", ptr);

pthread_mutex_lock(&sf_mutex);

#ifndef SFMALLOC_SAVE_SPACE
	sf.current_memory -= addr->bytes;
#endif

	sf.free_calls++;
pthread_mutex_unlock(&sf_mutex);

	free_fn(addr);
}

/*===========================================================================*
 *                                                                           *
 *                          HASHTABLE FUNCTIONS                              *
 *                                                                           *
 * From this point below all the macros, functions, structs belongs to the   *
 * hashtable.                                                                *
 *===========================================================================*/

/**
 * @brief Initializes the hashtable.
 *
 * @param ht Hashtable structure pointer to be
 * initialized.
 *
 * @return Returns 0 if success and a negative number
 * otherwise.
 */
static int hashtable_init(struct hashtable **ht)
{
	struct hashtable *out;
	out = calloc_fn(1, sizeof(struct hashtable));
	if (out == NULL)
		return (-1);

	out->capacity = HASHTABLE_DEFAULT_SIZE;
	out->elements = 0;
	out->bucket   = calloc_fn(out->capacity, sizeof(void *));
	if (out->bucket == NULL)
	{
		free_fn(out);
		return (-1);
	}

	*ht = out;
	return (0);
}

/**
 * @brief Deallocates the hashtable.
 *
 * @param ht Hashtable pointer.
 *
 * @return Returns 0 if success.
 */
static int hashtable_finish(struct hashtable **ht)
{
	struct hashtable *h;    /* Hashtable.    */
	struct address *l_ptr;  /* List pointer. */
	size_t i;               /* Loop index.   */

	h = *ht;

	/* Invalid array. */
	if (h == NULL)
		return (-1);

	/*
	 * For each bucket, deallocates each list.
	 */
	for (i = 0; i < h->capacity; i++)
	{
		struct address *l_ptr_next;

		l_ptr = h->bucket[i];
		while (l_ptr != NULL)
		{
			l_ptr_next = l_ptr->next;
			free_fn(l_ptr);
			l_ptr = l_ptr_next;
		}
	}

	free_fn(h->bucket);
	free_fn(h);
	return (0);
}

/**
 * @brief Calculates the bucket size accordingly with the
 * current hash function set and the current hashtable
 * capacity.
 *
 * @param ht Hashtable pointer.
 * @param key Key to be hashed.
 *
 * @return Returns a value between [0,h->capacity] that
 * should be used to address the target bucket for the
 * hash table.
 */
static size_t hashtable_bucket_index(struct hashtable **ht, uintptr_t key)
{
	struct hashtable *h;  /* Hashtable.    */
	uint64_t hash;        /* Hash value.   */
	size_t index;         /* Bucket index. */

	/*
	 * Its important to note that the hashtable->capacity
	 * will always be power of 2, thus, allowing the &-1
	 * as a modulus operation.
	 */
	h     = *ht;
	hash  = hashtable_splitmix64_hash(key, h->key_size);
	index = hash & (h->capacity - 1);

	return (index);
}

/**
 * @brief Resize the hashtable @p ht for the new given size
 * @p new_size.
 *
 * @param ht Hashtable to be resized.
 * @param new_size New hashtable size, must be power of two.
 *
 * @return Returns 0 if success, negative number otherwise.
 */
static int hashtable_resize(struct hashtable **ht, size_t new_size)
{
	struct hashtable *h;       /* Hashtable.    */
	struct address *l_ptr;     /* List pointer. */
	struct address *l_ptr_nxt; /* List pointer. */
	size_t i;                  /* Loop index.   */
	size_t hash;               /* Hash value.   */
	size_t old_capacity;
	struct address **new_buckets;

	h = *ht;

	/* Hashtable exists?. */
	if (h == NULL)
		return (-1);

	/* Is size power of two?. */
	if (new_size & (new_size - 1))
		return (-1);

	/* Allocate new buckets. */
	new_buckets = calloc_fn(new_size, sizeof(void *));
	if (new_buckets == NULL)
		return (-1);

	old_capacity  = h->capacity;
	h->capacity   = new_size;

#if HASHTABLE_DEBUG
	h->collisions = 0;
#endif

	/* Move elements to it. */
	for (i = 0; i < old_capacity; i++)
	{
		l_ptr = h->bucket[i];
		while (l_ptr != NULL)
		{
			l_ptr_nxt = l_ptr->next;

			if (l_ptr->address != 0)
			{
				/* Recalculates the hash for each key inside the bucket. */
				hash = hashtable_bucket_index(ht, l_ptr->address);
				l_ptr->next = new_buckets[hash];
#if HASHTABLE_DEBUG
				if(new_buckets[hash] != NULL)
					h->collisions++;
#endif
				new_buckets[hash] = l_ptr;
			}

			l_ptr = l_ptr_nxt;
		}
	}

	free_fn(h->bucket);
	h->bucket = new_buckets;
	return (0);
}

/**
 * @brief Adds the current @p key and @p value into the hashtable.
 *
 * @param ht Hashtable pointer.
 * @param key Key to be added.
 * @param value Value to be added.
 *
 * The hash table will grows twice whenever it reaches
 * its threshold of 60% percent.
 *
 * @return Returns 0 if success and a negative number otherwise.
 */
static int hashtable_add(struct hashtable **ht, struct address *key)
{
	struct hashtable *h;    /* Hashtable.    */
	struct address *l_ptr;  /* List pointer. */
	size_t hash;            /* Hash value.   */

	h = *ht;

	/* Hashtable exists?. */
	if (h == NULL)
		return (-1);

	/* Asserts if we have space enough. */
	if (h->elements >= h->capacity*HASHTABLE_LOAD_FACTOR)
		if (hashtable_resize(ht, h->capacity << 1) < 0)
			return (-1);

	/* Hash it. */
	hash = hashtable_bucket_index(ht, key->address);

	/*
	 * Loops through the list in order to see if the key already exists,
	 * if so, do nothing, just returns. However, this should not occur.
	 */
	l_ptr = h->bucket[hash];
	while (l_ptr != NULL)
	{
		if (l_ptr->address != 0 && hashtable_cmp_ptr(l_ptr->address, key->address) == 0)
			return (0);

		l_ptr = l_ptr->next;
	}

	/* Fill the entry. */
	key->next = h->bucket[hash];

	/* Add into the top of the buckets list. */
#if HASHTABLE_DEBUG
	if (h->bucket[hash] != NULL)
		h->collisions++;
#endif

	h->bucket[hash] = key;
	h->elements++;

	return (0);
}

/**
 * @brief Removes the address @p address from the hashtable.
 *
 * @param ht Hashtable pointer.
 * @param address Address to be removed.
 *
 * The hash table will shrinks twice whenever it reaches down
 * its threshold of 60% percent.
 *
 * @return If success, returns a valid pointer to 'struct address',
 * otherwise, returns NULL.
 */
static void *hashtable_remove(struct hashtable **ht, uintptr_t address)
{
	struct hashtable *h;    /* Hashtable.    */
	struct address *l_ptr;  /* List pointer. */
	struct address **prev;  /* Prev pointer. */
	size_t hash;            /* Hash value.   */

	h = *ht;

	/* Hashtable exists?. */
	if (h == NULL)
		return (NULL);

	/* Shall we contract the hashtable?. */
	if (h->elements <= ((h->capacity >> 1)*HASHTABLE_LOAD_FACTOR) &&
		h->capacity > HASHTABLE_DEFAULT_SIZE)
	{
		if (hashtable_resize(ht, h->capacity >> 1) < 0)
			return (NULL);
	}

	/* Hash it. */
	hash = hashtable_bucket_index(ht, address);

	/*
	 * Loops through the list in order to see if the key exists,
	 * if so, removes.
	 */
	prev  = &h->bucket[hash];
	l_ptr = h->bucket[hash];
	while (l_ptr != NULL)
	{
		if (l_ptr->address != 0 && hashtable_cmp_ptr(l_ptr->address, address) == 0)
		{
			*prev = l_ptr->next;

			h->elements--;
#if HASHTABLE_DEBUG
			if (l_ptr != h->bucket[hash])
				if (h->collisions)
					h->collisions--;
#endif
			return (l_ptr);
		}

		prev  = &l_ptr->next;
		l_ptr = l_ptr->next;
	}

	return (NULL);
}

/**
 * @brief Retrieves the value belonging to the parameter @p key.
 *
 * @param ht Hashtable pointer.
 * @param key Corresponding key to be retrieved.
 *
 * @return Returns the value belonging to the @p key, or NULL
 * if not found.
 */
static void* hashtable_get(struct hashtable **ht, uintptr_t key)
{
	struct hashtable *h;    /* Hashtable.    */
	struct address *l_ptr;  /* List pointer. */
	uint64_t hash;          /* Hash value.   */

	h = *ht;

	/* Hashtable exists and has at least one element?. */
	if (h == NULL || h->elements < 1)
		return (NULL);

	/* Hash it. */
	hash = hashtable_bucket_index(ht, key);

	/*
	 * Loops through the list in order to see if the key exists,
	 * if so, gets the value.
	 */
	l_ptr = h->bucket[hash];
	while (l_ptr != NULL)
	{
		if (l_ptr->address != 0 && hashtable_cmp_ptr(l_ptr->address, key) == 0)
			return (l_ptr);

		l_ptr = l_ptr->next;
	}

	/* If not found, return NULL. */
	return (NULL);
}

/*===========================================================================*
 *                         -.- Hash functions -.-                            *
 *===========================================================================*/

/**
 * @brief Generic key comparator.
 *
 * Compares two given pointers and returns a negative, 0 or positive
 * number if less than, equal or greater for the keys specified.
 *
 * @param key1 First key to be compared.
 * @param key2 Second key to be compared.
 *
 * @returns Returns a negative, 0 or positive number if @p key1
 * is less than, equal or greater than @p key2.
 */
static int hashtable_cmp_ptr(uintptr_t key1, uintptr_t key2)
{
	return ( (int)( key1 - key2 ) );
}

/*----------------------------------------------------------------------------*
 * Based in splitmix64, from Better Bit Mixing - Improving on MurmurHash3's   *
 * 64-bit Finalizer                                                           *
 *                                                                            *
 * Link:http://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html*
 * ----                                                                       *
 * I've found this particular interesting and the results showed that the     *
 * resulting hash is very close to MurMur3 ;-).                               *
 *----------------------------------------------------------------------------*/

/**
 * @brief splitmix64 based algorithm.
 *
 * This algorithm seems to be the 'Mix13' for the link above.
 *
 * @param key Key to be hashed.
 * @param size Pointer size (unused here).
 *
 * @return Returns a hashed number for the @p key argument.
 */
static uint64_t hashtable_splitmix64_hash(uintptr_t key, size_t size)
{
	uint64_t x; /* Hash key. */
	((void)size);

	x = (uint64_t)key;
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
	x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
	x = x ^ (x >> 31);
	return (x);
}

/*===========================================================================*
 *                               -.- Tests -.-                               *
 *===========================================================================*/

/* Array size test. */
#define ARRAY_SIZE  1024

/* Set to 1 to dump all buckets. */
#define DUMP_BUCKET 1

/* Considers each element as an integer value. */
#define BUCKET_AS_INTEGER 0

/**
 * @brief Print some statistics regarding the bucket usage, collisions
 * and etc, useful to know if the currently hash function used is
 * performing as expected.
 *
 * @param ht Hashtable pointer.
 */
#if HASHTABLE_ENABLE_STATS == 1
static void hashtable_print_stats(struct hashtable **ht)
{
	struct hashtable *h;        /* Hashtable.           */
	size_t used_buckets;        /* Used buckets.        */
	struct address *l_ptr;      /* List pointer.        */
	size_t max_bucket_size;     /* Max bucket size.     */
	size_t one_element_buckets; /* One-element buckets. */
	double elements_per_bucket; /* Elements per bucket. */
	double variance;            /* Variance.            */
	double stdev;               /* Standard Deviation.  */
	double mean;                /* Mean.                */
	size_t i;                   /* Loop index.          */

	/* Initialize. */
	max_bucket_size     = 0;
	one_element_buckets = 0;
	elements_per_bucket = 0;
	used_buckets        = 0;
	variance            = 0.0;
	stdev               = 0.0;
	mean                = 0.0;

	h = *ht;

	/* If valid hashtable. */
	if (h == NULL || h->elements < 1)
		return;

#if DUMP_BUCKET
	printf("Buckets:\n");
#endif
	for (i = 0; i < h->capacity; i++)
	{
		size_t bucket_size;
		bucket_size = 0;

#if DUMP_BUCKET
		printf("Bucket[%zu] = ", i);
#endif
		l_ptr = h->bucket[i];
		if (l_ptr != NULL)
			used_buckets++;

		while (l_ptr != NULL)
		{
			if (l_ptr->address != 0)
			{
				elements_per_bucket++;
#if DUMP_BUCKET
#if BUCKET_AS_INTEGER
				printf("%d ", *((int *)l_ptr->value) );
#else
				printf("0x%" PRIx64 " ", (uint64_t)l_ptr->address);
#endif
#endif
			}
			l_ptr = l_ptr->next;
			bucket_size++;
		}
#if DUMP_BUCKET
		printf("\n");
#endif
		if (bucket_size > max_bucket_size)
			max_bucket_size = bucket_size;
		if (bucket_size == 1)
			one_element_buckets++;
		if (bucket_size > 0)
			variance += (bucket_size * bucket_size);
	}

	mean     = elements_per_bucket / used_buckets;
	variance = (variance - (used_buckets * (mean*mean))) / (used_buckets - 1);
	stdev    = sqrt(variance);

	printf("--------------------------- Stats ---------------------------\n");
	printf("Buckets available: %zu\n", h->capacity);
	printf("Used buckets: %zu (%f%%)\n", used_buckets,
		((double)used_buckets/h->capacity)*100);
	printf("Max bucket size: %zu\n", max_bucket_size);
	printf("One element buckets: %zu (%f%%)\n", one_element_buckets,
		((double)one_element_buckets/used_buckets)*100);
	printf("Mean elements per bucket: %f\n", mean);
	printf("    Variance: %f\n", variance);
	printf("    Standard Deviation: %f\n", stdev);
#if HASHTABLE_DEBUG
	printf("Collisions: %zu, elements: %zu\n", h->collisions, h->elements);
#endif
	printf("-------------------------------------------------------------\n");
}
#endif
