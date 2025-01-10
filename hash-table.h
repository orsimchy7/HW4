#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked-list.h"

/* Make the code cleaner - use typedefs */
typedef void* ht_key_t;
typedef void* ht_val_t;

/* typename of hash-table user functions */
typedef int(*hash_func_t)(ht_key_t, int); /* Converts a key to a number in [0,N-1] */

struct ht; /* Hash-Table: opaque pointer */

struct ht* hash_table_init(hash_func_t hash_key,        
                           clone_func_t clone_key,
                           destroy_func_t destroy_key,
                           compare_func_t compare_key,
                           clone_func_t clone_val,
                           destroy_func_t destroy_val,
                           int N);
void hash_table_destroy(struct ht *ht);

void hash_table_insert(struct ht *ht, ht_key_t key, ht_val_t val);
ht_val_t hash_table_lookup(struct ht *ht, ht_key_t key);


#endif
