#ifndef DISK_H
#define DISK_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Statically sized hash table
 * used for implementing database
 */

typedef uint16_t HashKey;
typedef struct Bucket Bucket;
typedef struct HashTableNode HashTableNode;
typedef HashTableNode *HashTable;

struct Bucket {
  HashKey key;
  const void *data;
  Bucket *next;
};

struct HashTableNode {
  Bucket *buckets;
  size_t buckets_count;
};

#define HASH_TABLE_SIZE 1000
#define MAX_OVERFLOW_CHAINING 3

inline static HashKey key_hash(const char *key_str) {
  char *end;
  const long key_long = strtol(key_str, &end, 0);
  const HashKey key_uint = (HashKey)(key_long % HASH_TABLE_SIZE);
  return key_uint;
}

static void bucket_insert(HashTable *table, Bucket bucket) {
  const HashKey key = bucket.key;
  assert(key < HASH_TABLE_SIZE);

  HashTableNode *node = table[key];
  assert(node->buckets_count + 1 < MAX_OVERFLOW_CHAINING);

  bucket.next = &node->buckets[node->buckets_count];
  node->buckets[node->buckets_count++] = bucket;
}
static Bucket bucket_new(char *key_str, const void *data) {
  HashKey key = key_hash(key_str);
  Bucket b = {.key = key, .data = data};

  return b;
}

void table_insert(HashTable *table, char *key_str, const void *data) {
  Bucket bucket = bucket_new(key_str, data);
  bucket_insert(table, bucket);
}

const void *table_get(const HashTable *table, char *key_str) {
  const HashKey key = key_hash(key_str);

  HashTableNode *node = table[key];

  for (size_t i = 0; i < node->buckets_count; i++) {
    Bucket *bucket = &node->buckets[i];
    if (bucket->key == key) {
      return bucket->data;
    }
  }

  return NULL;
}

void table_delete(const HashTable *table, char *key_str) {
  const HashKey key = key_hash(key_str);

  HashTableNode *node = table[key];

  for (size_t i = 0; i < node->buckets_count; i++) {
    Bucket *bucket = &node->buckets[i];
    if (bucket->key == key) {
      node->buckets_count--;
    }
  }
}

#endif
