#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "disk.h"
#include <stdlib.h>

typedef struct {
  Table table;

} Snapshot;

Snapshot snapshot_initiate(time_t *time, Table *src, char **names,
                           size_t names_count);

#endif
