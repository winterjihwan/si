#ifndef DISK_H
#define DISK_H

#include "table.h"
#include <stdio.h>
#include <time.h>

#define DISK_TABLE_CAPACITY 5

typedef struct Table Table;
struct Table {
  char *name;
  HashTable table;
};

typedef struct {
  Table database[DISK_TABLE_CAPACITY];
  size_t table_count;
} Disk;

typedef struct {
  time_t version;
  char *name;
  char *data;
} Resource;

void disk_insert(Disk *disk, char *table_name, const Resource *resource);
Table *disk_table_new(Disk *disk, char *table_name);
Resource *disk_table_read(Table *table, char *key_str);

void resource_print(Resource *resource);
Resource resource_new(time_t version, char *name, char *data);

#endif
