#ifndef TABLE_H
#define TABLE_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Statically sized hash table
 * used for implementing database
 */

typedef uint16_t HashKey;
typedef struct Bucket Bucket;
typedef struct HashTableNode HashTableNode;

struct Bucket {
  HashKey key;
  void *data;
  unsigned long data_size;
  Bucket *next;
};

#define MAX_OVERFLOW_CHAINING 3

struct HashTableNode {
  Bucket buckets[MAX_OVERFLOW_CHAINING];
  size_t buckets_count;
};

#define HASH_TABLE_SIZE 50

typedef struct {
  HashTableNode nodes[HASH_TABLE_SIZE];
} HashTable;

inline static HashKey key_hash(const char *key_str) {
  HashKey key = 5381;

  for (size_t i = 0; key_str[i] != '\0'; i++) {
    key = ((key << 5) + key) + key_str[i];
  }

  return key;
}

void hash_table_insert(HashTable *table, const char *key_str, const void *data,
                       const unsigned long data_size);
void hash_table_update(HashTable *table, const char *key_str, void *new_data,
                       const unsigned long new_data_size);
const void *hash_table_get(HashTable *table, char *key_str);
void hash_table_delete(const HashTable *table, char *key_str);
void hash_table_dump(const HashTable *table);

static inline Bucket *hash_table_bucket_get(HashTable *table,
                                            const char *key_str);

#endif
