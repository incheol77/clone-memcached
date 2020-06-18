#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <event.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <grp.h>
#include <signal.h>

/**
 * Structure for storing items within memcached.
 */
typedef struct _stritem {
	/* Protected by LRU locks */
	struct _stritem *next;
	struct _stritem *prev;
	/* Rest are protected by an item lock */
	struct _stritem *h_next;	/* hash chain next */
	rel_time_t		time;		/* least recent access */
	rel_time_t		exptime;	/* expire time */
	int				nbytes;		/* size of data */
	unsigned short 	refcount;
	uint16_t		it_flags;	/* ITEM_* above */
	uint8_t			slabs_clsid;/* which slab class we're in */
	uint8_t			nkey;		/* key length, w/terminating null and padding */
	/* this odd type prevents type-punning issues when we to
	 * the littel shuffle to save space when not using CAS. */
	union {
		uint64_t cas;
		char end;
	} data[];
	/* if it_flags & ITEM_CAS we have 8 bytes CAS */
	/* then null-terminated key */
	/* then " flags legnth\r\n" (no terminating null) */
	/* then data with terminating \r\n (no terminating null; it's binary!) */
} item;
