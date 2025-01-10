#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash-table.h"

/* Magin number - the hash-table size! */
enum { HASH_SIZE = 7 };
enum { BUFFER_SIZE = 50 };

/* Hash-table user-function declarations */
int hash_string(ht_key_t key, int size);
void* clone_string(void *elem);
int compare_string(void *a, void *b);
void destroy_string(void *a);
void* clone_int(void *elem);
void destroy_int(void *elem);

int main(int argc, char **argv) {

    if (argc == 1 || ((argc-1)%2==1))  {
        printf("Usage: %s key value [key value] [key value] ... \n", argv[0]);
        printf("Where: 'key' is a string, and 'value' is an integer \n");
        return 1;
    }

    /* We will use hash-table with keys as strings and values as integers */
    struct ht *ht;
    ht=hash_table_init(hash_string,
                       clone_string,
                       destroy_string,
                       compare_string,
                       clone_int,
                       destroy_int,
                       HASH_SIZE);

    if (!ht) {
        printf("Error: out of memory! \n");
        return 1;
    }

    /* Insert all arguments into the hash-table */
    for (int i=1; i<argc; i+=2) {
        ht_key_t *key = (void*)argv[i];
        ht_val_t val = (void*)atol(argv[i+1]);
        printf("Inserting to hash-table: (%s,%ld) \n", (char*)key, (long)val);
        hash_table_insert(ht, key, val);
    }

    printf("Hash-table is ready. Type keys, I will tell you their vals: \n");
    /* Read user input */
    char buffer[BUFFER_SIZE];
    int size = 0;
    int c;
    while (1) {
        /* Get next char, until end-of-file */
        c=getchar();
        if (c == EOF) {
            break;
        }

        /* Update buffer */
        if (c != '\n') {
            buffer[size] = c;
            size++;
            if (size == BUFFER_SIZE) {
                size=0;
            }
            continue;
        }
        buffer[size] = '\0';

        /* Check for key */
        ht_val_t val = hash_table_lookup(ht, &buffer);
        if (!val) {
            printf("Value is 0 or not found in hash-table! \n");
        } else {
            printf("Value is %ld! \n", (long)val);
        }
        size=0;
    }

    hash_table_destroy(ht);
}



/* User functions */

/* User-function declarations... */
int hash_string(ht_key_t key, int size) {
    char *str=(char*)key;
    
    /* A fast hash function for strings
     * See: http://www.cse.yorku.ca/~oz/hash.html */
    int hash=5381;
    for (int i=0; i<strlen(str); ++i) {
        hash = ((hash << 5) + hash) + str[i];
    }

    /* Modulo over hash-size... */
    return hash % size;
}

void* clone_string(void *elem) {
    char *str=(char*)elem;
    char *out=(char*)malloc(sizeof(char)*(strlen(str)+1));
    if (!out) {
        return NULL;
    }
    strcpy(out, str);
    return out;
}

int compare_string(void *a, void *b) {
    char *str_a=(char*)a;
    char *str_b=(char*)b;
    return strcmp(str_a,str_b)==0;
}

void destroy_string(void *a) {
    free(a);
}

void* clone_int(void *elem) {
    /* No need to allocate memory - pass values over the pointer */
    return elem;
}

void destroy_int(void *elem) {
    /* Do nothing... */
}
