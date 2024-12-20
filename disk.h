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
  char *data;
} Resource;

void disk_table_new(Disk *disk, char *table_name);

void disk_insert(Disk *disk, char *table_name, char *key_str,
                 const Resource *resource);

Resource *disk_read(Disk *disk, char *table_name, char *key_str);

void resource_print(char *name, Resource *resource);

#endif
