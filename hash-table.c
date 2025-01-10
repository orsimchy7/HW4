#include <stdlib.h>
#include "hash-table.h"

/* The hash-table */
struct ht {
	/* Array of lists */
	struct list **entries;
	int size;
	/* Store all user-function pointers here, we will need them later on! */
	hash_func_t hash_key;
	clone_func_t clone_key;
	destroy_func_t destroy_key;
	compare_func_t compare_key;
	clone_func_t clone_val;
	destroy_func_t destroy_val;
};

/* Key-value pair. The hash-table linked-lists will have these
 * as elements! */
struct kv_pair {
	ht_key_t key;
	ht_val_t val;
	struct ht *ht; /* Pointer to parent - to access the user-functions! */
};

/* We need to define a set of user-functions for the linked list... */
void *kv_clone(void *elem) {

	struct kv_pair *kv_pair;
	struct kv_pair *out;
	ht_key_t key_clone;
	ht_val_t val_clone;

	kv_pair=(struct kv_pair*)elem;

	out=(struct kv_pair*)malloc(sizeof(*out));

	/* Clone the key and the value using the user functions... */
	key_clone=kv_pair->ht->clone_key(kv_pair->key);
	val_clone=kv_pair->ht->clone_val(kv_pair->val);

	if (!out || !key_clone || !val_clone) {
		/* Out of memory... */
		free(out);
		free(key_clone);
		free(val_clone);
		return NULL;
	}

	out->ht=kv_pair->ht;
	out->key=key_clone;
	out->val=val_clone;
	return out;
}

void kv_destroy(void *elem) {
	if (!elem) {
		return;
	}

	struct kv_pair *kv_pair;
	kv_pair=(struct kv_pair*)elem;

	/* Destroy the key and val using ht user functions */
	kv_pair->ht->destroy_key(kv_pair->key);
	kv_pair->ht->destroy_val(kv_pair->val);
	free(kv_pair);
}

int kv_compare(void *a, void *b) {
	/* Check valid inputs */
	if (!a || !b) {
		return 0;
	}

	struct kv_pair *kv_a=(struct kv_pair*)a;
	struct kv_pair *kv_b=(struct kv_pair*)b;

	/* Do the ky-paris are owned by the same hash-table ?! */
	if (kv_a->ht != kv_b->ht) {
		return 0;
	}

	/* Compare only keys (!) Using ht user-functions */
	return kv_a->ht->compare_key(kv_a->key, kv_b->key);
}



struct ht* hash_table_init(hash_func_t hash_key,        
                           clone_func_t clone_key,
                           destroy_func_t destroy_key,
                           compare_func_t compare_key,
                           clone_func_t clone_val,
                           destroy_func_t destroy_val,
                           int N) {
	struct ht *ht;
	struct list **entries;
	int error;

	ht=(struct ht*)malloc(sizeof(*ht));
	entries=(struct list**)malloc(sizeof(struct list*)*N);
	if (!ht || !entries) {
		/* Out of memory - must free what was allocated */
		free(ht);
		free(entries);
		return NULL;
	}

	ht->size = N;
	ht->entries = entries;
	ht->hash_key=hash_key;
    ht->clone_key=clone_key;
    ht->destroy_key=destroy_key;
    ht->compare_key=compare_key;
    ht->clone_val=clone_val;
    ht->destroy_val=destroy_val;

	/* Create the lists... */
	error = 0;
	for (int i=0; i<N; ++i) {
		/* Initiate the list using kv user functions! */
		entries[i] = list_init(kv_clone, kv_destroy, kv_compare);
		/* Check for valid list */
		if (!entries[i]) {
			error = 1;
		}
	}

	/* On error - destroy the hash-table */
	if (error) {
		hash_table_destroy(ht);
		return NULL;
	}

	return ht;
}

void hash_table_destroy(struct ht *ht) {
	if (!ht) {
		return;
	}
	for (int i=0; i<ht->size; ++i) {
		/* Will also destroy all the kv-paris in list... how? */
		list_destroy(ht->entries[i]);
	}
	free(ht->entries);
	free(ht);
}

void hash_table_insert(struct ht *ht, ht_key_t key, ht_val_t val) {
	/* Check for invalid arguments... */
	if (!ht || !key || !val) {
		return;
	}

	struct kv_pair *kv_pair;

	/* Create new kv-pair */
	kv_pair=(struct kv_pair*)malloc(sizeof(*kv_pair));
	if (!kv_pair) {
		/* Out of memory... */
		return;
	}
	kv_pair->ht = ht;
	kv_pair->key = ht->clone_key(key);
	kv_pair->val = ht->clone_val(val);
	if (!kv_pair->key || !kv_pair->val) {
		/* Error clonning... */
		free(kv_pair->key);
		free(kv_pair->val);
		free(kv_pair);
		return;
	}

	int hash = ht->hash_key(key, ht->size);
	list_push(ht->entries[hash], kv_pair);
}

ht_val_t hash_table_lookup(struct ht *ht, ht_key_t key) {
	/* Check for invalid arguments... */
	if (!ht || !key) {
		return NULL;
	}

	/* Get the relevant list in the table */
	int hash = ht->hash_key(key, ht->size);
	struct list *list = ht->entries[hash];

	/* Super delicate. Remember: the linked-list compare "nodes" (kv-pairs)
	 * using "kv_compare", that in turn, compares kv-pairs only by their keys!
	 * Therefore, we will make a "dummy" ky-pair with the key to search! */
	struct kv_pair kv_pair;
	kv_pair.key = key;  /* No need to clone... */
	kv_pair.val = NULL; /* Dummy value */
	kv_pair.ht = ht;

	struct kv_pair *found;
	found = list_search(list, &kv_pair);
	if (!found) {
		return NULL;
	}

	/* Returns by pointer */
	return found->val;
}