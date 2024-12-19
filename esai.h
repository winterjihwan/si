#ifndef ESAI_H
#define ESAI_H

#include <time.h>

#define MAX_TIME 0x7FFFFFFE
#define MAX_GLOBAL_TXS 10
#define MAX_RESOURCES 5
#define MAX_ACTIONS 10
#define MAX_LOGS 100
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
  char *name;
  char *data;
  time_t write_time;
} Resource;

typedef enum { READ, WRITE } action_t;
typedef struct {
  time_t time;
  action_t type;
  Resource *rs;
} Action;

typedef long tx_id;
typedef enum { STARTED, COMMITTED, ENDED } tx_state;
typedef struct {
  char *name;
  tx_id id;
  tx_state state;

  time_t start_time;
  time_t commit_time;
  time_t end_time;

  Action actions[10];
  size_t actions_count;
} Tx;

typedef enum {
  BEGIN_TX_LOG,
  WRITE_LOG,
  READ_LOG,
  RECOVER_LOG,
  ABORT_TX_LOG,
  COMMIT_TX_LOG
} log_t;
typedef struct {
  log_t type;
  time_t time;
  tx_id tx_id;
  Resource *rs;
  char *prev;
  char *after;
} Log;

int tx_should_compare(const Tx *t1, const Tx *t2);
int tx_is_conflict(const Tx *tx);

const char *action_t_to_string(const action_t action_t);
const char *log_t_to_string(const log_t log_t);

void tx_print(Tx *tx);
void rs_print(Resource *rs);
void log_print(const Log *log);

void tx_read(Tx *tx, Resource *rs);
void tx_write(Tx *tx, Resource *rs, char *new_data);
void tx_abort(Tx *tx);
void tx_commit(Tx *tx);

void global_txs_dump(void);
void resources_dump(void);
void stable_storage_dump(void);
void tx_schedule_dump(const Tx *t1, const Tx *t2);

#endif
