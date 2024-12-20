#ifndef TABLE_H
#define TABLE_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Statically sized hash table
 * used for implementing database
 */

typedef uint16_t HashKey;
typedef struct Bucket Bucket;
typedef struct HashTableNode HashTableNode;

struct Bucket {
  HashKey key;
  const void *data;
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

static void bucket_insert(HashTable *table, Bucket bucket) {
  const HashKey key_index = bucket.key % HASH_TABLE_SIZE;

  assert(key_index < HASH_TABLE_SIZE);

  HashTableNode *node = &table->nodes[key_index];
  assert(node->buckets_count + 1 < MAX_OVERFLOW_CHAINING);

  bucket.next = &node->buckets[node->buckets_count];
  node->buckets[node->buckets_count++] = bucket;
}

static Bucket bucket_new(char *key_str, const void *data) {
  HashKey key = key_hash(key_str);
  Bucket b = {.key = key, .data = data};

  return b;
}

void hash_table_insert(HashTable *table, char *key_str, const void *data);
const void *hash_table_get(const HashTable *table, char *key_str);
void hash_table_delete(const HashTable *table, char *key_str);

#endif
