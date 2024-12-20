#include "disk.h"
#include "table.h"
#include <assert.h>
#include <stdio.h>

Table *disk_table_new(Disk *disk, char *table_name) {
  assert(disk->table_count + 1 < DISK_TABLE_CAPACITY);
  Table table = {
      .name = table_name,
  };

  disk->database[disk->table_count] = table;
  Table *p_table = &disk->database[disk->table_count++];

  return p_table;
}

Resource *disk_table_read(Table *table, char *key_str) {
  return (Resource *)hash_table_get(&table->table, key_str);
}

void disk_table_write(Table *table, char *key_str, char *new_data) {
  hash_table_update(&table->table, key_str, new_data);
}

void disk_table_insert(Table *table, const Resource resource) {
  hash_table_insert(&table->table, resource.name, (void *)&resource,
                    sizeof(resource));
}

// assumes resource->data is of type char*
void disk_table_dump(const Table *table) {
  printf("Table: \n");

  for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
    const HashTableNode *node = &table->table.nodes[i];

    for (size_t j = 0; j < node->buckets_count; j++) {
      Resource *rs = (Resource *)node->buckets[j].data;

      resource_print(rs);
    }
  }

  printf("\n");
}

void resource_print(const Resource *resource) {
  printf("Resource:\n");
  printf("\tversion: %ld\n", resource->version);
  printf("\tname: %s\n", resource->name);
  printf("\tdata: %s\n", resource->data);
}

Resource resource_new(time_t version, char *name, char *data) {
  return (Resource){
      .version = version,
      .name = name,
      .data = data,
  };
}
