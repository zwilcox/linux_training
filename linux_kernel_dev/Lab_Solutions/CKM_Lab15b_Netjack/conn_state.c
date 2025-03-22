#include <linux/hashtable.h>
#include <linux/slab.h>

#include "conn_state.h"

static DEFINE_HASHTABLE(conn_table, 8);
static DEFINE_SPINLOCK(conn_state_lock);

conn_state_t* conn_state_create(unsigned long key) {
	conn_state_t* cs;
	cs = kmalloc(sizeof(conn_state_t), GFP_KERNEL);
	if (cs == NULL) {
		printk(KERN_ALERT "Failed in kmalloc\n");
		return NULL;
	}
	cs->key = key;
	spin_lock(&conn_state_lock);
	hash_add(conn_table, &cs->hash, cs->key);
	spin_unlock(&conn_state_lock);
	return cs;
}

void conn_state_delete_all(void) {
	int bkt;
	conn_state_t* it;
	struct hlist_node tmp;
	struct hlist_node* tmpptr;
	tmpptr = &tmp;
	spin_lock(&conn_state_lock);
	hash_for_each_safe(conn_table, bkt, tmpptr, it, hash) {
		hash_del(&it->hash);
		kfree(it);
	}
	spin_unlock(&conn_state_lock);
}

void conn_state_delete(unsigned long key) {
	conn_state_t* it;
	spin_lock(&conn_state_lock);
	hash_for_each_possible(conn_table, it, hash, key) {
		if (it->key == key) {
			hash_del(&it->hash);
			kfree(it);
			break;
		}
	}
	spin_unlock(&conn_state_lock);
	return;
}

conn_state_t* conn_state_get(unsigned long key) {
	conn_state_t* cs;
	conn_state_t* it;
	cs = NULL;
	spin_lock(&conn_state_lock);
	hash_for_each_possible(conn_table, it, hash, key) {
		if (it->key == key) {
			cs = it;
			break;
		}
	}
	spin_unlock(&conn_state_lock);
	return cs;
}

