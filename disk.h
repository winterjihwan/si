#ifndef DISK_H
#define DISK_H

#include "table.h"
#include <stdio.h>
#include <time.h>

#define DISK_TABLE_CAPACITY 5
#define MAX_KEYS_CAPACITY 100

typedef struct Table Table;
struct Table {
  char *name;
  HashTable table;
};

typedef struct {
  Table database[DISK_TABLE_CAPACITY];
  size_t table_count;
} Disk;

static Disk LOGICAL_DISK = {0};

typedef struct {
  time_t version;
  char *name;
  char *data;
} Resource;

Table *disk_table_new(Disk *disk, char *table_name);
Resource *disk_table_read(Table *table, char *key_str);
void disk_table_write(Table *table, char *key_str, void *new_data);
void disk_table_insert(Table *table, const Resource resource);
void disk_table_dump(const Table *table);

void resource_print(const Resource *resource);
Resource resource_new(time_t version, char *name, char *data);

#endif
