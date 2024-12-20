#include "disk.h"
#include "table.h"
#include <assert.h>
#include <stdio.h>

static Disk DISK = {0};
static time_t GLOBAL_CLOCK = {0};

void disk_table_new(Disk *disk, char *table_name) {
  assert(disk->table_count + 1 < DISK_TABLE_CAPACITY);
  Table table = {
      .name = table_name,
  };

  disk->database[disk->table_count++] = table;
}

void disk_insert(Disk *disk, char *table_name, char *key_str,
                 const Resource *resource) {
  for (size_t i = 0; i < DISK_TABLE_CAPACITY; i++) {
    Table *table = &disk->database[i];
    if (table->name != table_name)
      continue;

    hash_table_insert(&table->table, key_str, resource);
    return;
  }
}

Resource *disk_read(Disk *disk, char *table_name, char *key_str) {
  for (size_t i = 0; i < DISK_TABLE_CAPACITY; i++) {
    Table *table = &disk->database[i];
    if (table->name != table_name)
      continue;

    return (Resource *)hash_table_get(&table->table, key_str);
  }

  return NULL;
}

void resource_print(char *name, Resource *resource) {
  printf("Resource %s\n", name);
  printf("\tversion: %ld\n", resource->version);
  printf("\tdata: %s\n", resource->data);
}

int main(void) {
  char *table_name = "Table-A";
  disk_table_new(&DISK, table_name);
  Resource recordA = {.version = GLOBAL_CLOCK++, .data = "I prefer 2PL"};

  char X = 'X';
  disk_insert(&DISK, table_name, &X, &recordA);
  Resource *read = disk_read(&DISK, table_name, &X);
  resource_print(&X, read);
}
