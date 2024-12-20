#ifndef ESAI_H
#define ESAI_H

#include "disk.h"
#include <time.h>

#define MAX_TIME 0x7FFFFFFE
#define MAX_GLOBAL_TXS 10
#define MAX_RESOURCES 5
#define MAX_ACTIONS 10
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef enum { READ, WRITE } action_t;
typedef struct {
  time_t time;
  action_t type;
  Resource *rs;
} Action;

typedef long TxId;
typedef enum { STARTED, COMMITTED, ENDED } tx_t;
typedef struct {
  char *name;
  TxId id;
  tx_t state;

  time_t start_time;
  time_t commit_time;
  time_t end_time;

  Action actions[10];
  size_t actions_count;
} Tx;

int tx_should_compare(const Tx *t1, const Tx *t2);
int tx_is_conflict(const Tx *tx);

const char *action_t_to_string(const action_t action_t);

void tx_print(Tx *tx);
void rs_print(Resource *rs);

void tx_read(Tx *tx, Table *table, char *rs_name);
void tx_write(Tx *tx, Table *table, char *rs_name, char *new_data);
void tx_abort(Tx *tx);
void tx_commit(Tx *tx);

void global_txs_dump(void);
void resources_dump(void);
void tx_schedule_dump(const Tx *t1, const Tx *t2);

#endif
