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

void disk_insert(Disk *disk, char *table_name, const Resource *resource) {
  for (size_t i = 0; i < DISK_TABLE_CAPACITY; i++) {
    Table *table = &disk->database[i];
    if (table->name != table_name)
      continue;

    hash_table_insert(&table->table, resource->name, resource);
    return;
  }
}

void resource_print(Resource *resource) {
  printf("Resource %s\n", resource->name);
  printf("\tversion: %ld\n", resource->version);
  printf("\name: %s\n", resource->name);
  printf("\tdata: %s\n", resource->data);
}

Resource resource_new(time_t version, char *name, char *data) {
  return (Resource){
      .version = version,
      .name = name,
      .data = data,
  };
}
