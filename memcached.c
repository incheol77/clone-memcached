#include "memcached.h"
#ifdef EXTSTORE
#include "storage.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifdef __sun
#include <atomic.h>
#endif

#ifdef TLS
#include <openssl/ssl.h>
#endif

#define ITEMS_PER_ALLOC 64

/* An item in the connection queue. */
enum conn_queue_item_modes {
	queue_new_conn,		/* brand new connection. */
};

typedef struct conn_queue_item CQ_ITEM;
struct conn_queue_item {
	int	sfd;
	enum conn_states init_state;
	int event_flags;
	int read_buffer_size;
	enum newwork_transport transport;
	enum conn_queue_item_modes mode;
	conn *c;
	void *ssl;
	CQ_ITEM *next;
};

/* A connection queue. */
typedef struct conn_queue CQ;
struct conn_queue {
	CQ_ITEM *head;
	CQ_ITEM *tail;
	pthread_mutex_t lock;
};

