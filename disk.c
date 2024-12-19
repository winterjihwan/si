#include "disk.h"
#include <stdio.h>

static HashTable HASH_TABLE = {0};

int main(void) {
  table_insert(&HASH_TABLE, "A", "Atomicity");

  printf("Retrieved: %s\n", (char *)table_get(&HASH_TABLE, "A"));
}
