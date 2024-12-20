#include "disk.h"
#include "tx.h"

time_t TIME = 0x01;
Disk DISK = {0};

int main(void) {
  Resource r1 = resource_new(TIME++, "X", "Hi goblin!");
  Resource r2 = resource_new(TIME++, "Y", "Harry Potter!");
  Table *tableA = disk_table_new(&DISK, "Table A");
  disk_table_insert(tableA, r1);
  disk_table_insert(tableA, r2);

  /*-----------------------------------------------------*/

  /*---Abort---*/

  // t1: begin()
  Tx *t1 = tx_new("T1", tableA, (char *[]){"X"}, 1);

  // t1: read(x)
  tx_read(t1, "X");

  // t1: write(x)
  tx_write(t1, "X", "Hi angel!");

  // t1: commit()
  tx_commit(t1, "Table A");

  // t2: begin()
  Tx *t2 = tx_new("T2", tableA, (char *[]){"X"}, 1);

  // t2: read(x)
  tx_read(t2, "X");

  // t2: write(x)
  tx_write(t2, "X", "Welcome to Seoul!");

  // t2: commit()
  tx_commit(t2, "Table A");

  disk_table_dump(tableA);
  tx_schedule_dump(t1, t2);

  /*---Abort---*/

  // t1: begin()
  Tx *t3 = tx_new("T3", tableA, (char *[]){"Y"}, 1);

  // t1: read(x)
  tx_read(t3, "Y");

  // t1: write(x)
  tx_write(t3, "Y", "Lord of the rings");

  // t2: begin()
  Tx *t4 = tx_new("T3", tableA, (char *[]){"Y"}, 1);

  // t2: read(x)
  tx_read(t4, "Y");

  // t2: write(x)
  tx_write(t4, "Y", "Kingsman");

  // t1: commit()
  tx_commit(t3, "Table A");

  // t2: commit()
  tx_commit(t4, "Table A");

  disk_table_dump(tableA);
  tx_schedule_dump(t1, t2);
}
