#include "snapshot.h"
#include "disk.h"
#include "table.h"
#include <time.h>

Snapshot snapshot_initiate(time_t *time, Table *src, char **names,
                           size_t names_count) {
  Table dest = {.name = (char *)"Dangling Workspace"};

  // TODO: refactor using hashmap keys
  // No point of using hashmap if iterating O(n^3)
  for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
    const HashTableNode *node = &src->table.nodes[i];

    for (size_t j = 0; j < node->buckets_count; j++) {
      for (size_t k = 0; k < names_count; k++) {
        const Bucket *bucket = &node->buckets[k];

        HashKey key = key_hash(names[k]);
        if (bucket->key == key) {
          Resource *resource = (Resource *)bucket->data;
          if (resource->version > *time)
            continue;

          void *temp = malloc(sizeof(*resource));
          memcpy(temp, resource, sizeof(*resource));
          disk_table_insert(&dest, *(Resource *)temp);
        }
      }
    }
  }

  return (Snapshot){.table = dest};
}
