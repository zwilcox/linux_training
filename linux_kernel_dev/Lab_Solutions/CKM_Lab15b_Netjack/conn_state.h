#ifndef CONN_STATE_H
#define CONN_STATE_H

typedef struct conn_state {
	unsigned long key;
	struct hlist_node hash;
	/* Place state variables you
	 * want to associate permanently
	 * with a socket here */
} conn_state_t;


/* Creates a connection state struct and places it in the
 * global hashmap.
 * key can be whatever you want, but it must be unique
 * per socket. Pointers to socket and sock structs should
 * work fine */
conn_state_t* conn_state_create(unsigned long key);

/* Removes all connection states from the global hashmap
 * and frees them */
void conn_state_delete_all(void);

/* Removes the single connection state associated with the
 * given key from the global hashmap and frees it */
void conn_state_delete(unsigned long key);

/* Retrieves a pointer to a connection state associated with
 * the given key from the global hashmap */
conn_state_t* conn_state_get(unsigned long key);


#endif
