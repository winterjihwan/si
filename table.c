#include "table.h"
#include <stdio.h>

void hash_table_insert(HashTable *table, char *key_str, const void *data) {
  Bucket bucket = bucket_new(key_str, data);
  bucket_insert(table, bucket);
}

const void *hash_table_get(const HashTable *table, char *key_str) {
  const HashKey key = key_hash(key_str);

  HashTableNode node = table->nodes[key % HASH_TABLE_SIZE];

  for (size_t i = 0; i < node.buckets_count; i++) {
    Bucket *bucket = &node.buckets[i];
    if (bucket->key == key) {
      return bucket->data;
    }
  }

  return NULL;
}

void bucket_print(const Bucket *bucket) {
  printf("Bucket: \n");
  printf("\tkey: %d\n", bucket->key);
  printf("\tdata: %s\n", (char *)bucket->data);
}

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
