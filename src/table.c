#include "table.h"
#include <stdio.h>
#include <string.h>

static void bucket_insert(HashTable *table, Bucket bucket) {
  const HashKey key_index = bucket.key % HASH_TABLE_SIZE;

  assert(key_index < HASH_TABLE_SIZE);

  HashTableNode *node = &table->nodes[key_index];
  assert(node->buckets_count + 1 < MAX_OVERFLOW_CHAINING);

  bucket.next = &node->buckets[node->buckets_count];
  node->buckets[node->buckets_count++] = bucket;
}

static Bucket bucket_new(const char *key_str, const void *data,
                         const unsigned long data_size) {
  HashKey key = key_hash(key_str);

  void *data_copy = malloc(data_size);
  memcpy(data_copy, data, data_size);

  Bucket b = {.key = key, .data = data_copy, .data_size = data_size};

  return b;
}

void hash_table_insert(HashTable *table, const char *key_str, const void *data,
                       const unsigned long data_size) {
  Bucket bucket = bucket_new(key_str, data, data_size);
  bucket_insert(table, bucket);
}

static inline Bucket *hash_table_bucket_get(HashTable *table,
                                            const char *key_str) {
  const HashKey key = key_hash(key_str);

  HashTableNode *node = &table->nodes[key % HASH_TABLE_SIZE];

  for (size_t i = 0; i < node->buckets_count; i++) {
    Bucket *bucket = &node->buckets[i];
    if (bucket->key == key) {
      return bucket;
    }
  }

  fprintf(stderr, "HASH_TABLE: hash_table_bucket_get(), Entry non exist");
  abort();
}

void bucket_print(const Bucket *bucket) {
  printf("Bucket: \n");
  printf("\tkey: %d\n", bucket->key);
  printf("\tdata: %s\n", (char *)bucket->data);
}

void hash_table_update(HashTable *table, const char *key_str, void *new_data,
                       const unsigned long new_data_size) {
  Bucket *bucket = hash_table_bucket_get(table, key_str);

  void *data_copy = malloc(new_data_size);
  memcpy(data_copy, new_data, new_data_size);
  bucket->data = data_copy;
}

const void *hash_table_get(HashTable *table, char *key_str) {
  Bucket *bucket = hash_table_bucket_get(table, key_str);
  return bucket->data;
}

// assumes data: char*
void hash_table_dump(const HashTable *table) {
  printf("Table: \n");

  for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
    const HashTableNode *node = &table->nodes[i];

    for (size_t j = 0; j < node->buckets_count; j++) {
      bucket_print(&node->buckets[j]);
    }
  }

  printf("\n");
}

void hash_table_delete(const HashTable *table, char *key_str) {
  const HashKey key = key_hash(key_str);

  HashTableNode node = table->nodes[key];

  for (size_t i = 0; i < node.buckets_count; i++) {
    Bucket *bucket = &node.buckets[i];
    if (bucket->key == key) {
      node.buckets_count--;
    }
  }
}
