#include "assert.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_TIME 0x7FFFFFFE
#define MAX_GLOBAL_TXS 10
#define MAX_RESOURCES 5
#define MAX_ACTIONS 10
#define MAX_LOGS 100

typedef struct {
  char *data;
  time_t write_time;
  time_t read_time;
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

typedef enum { WRITE_LOG, READ_LOG } log_t;
typedef struct {
  log_t type;
  time_t time;
  tx_id tx_id;
  Resource *rs;
  char *prev;
  char *after;
} Log;

static time_t TIME = 0x01;
static Tx GLOBAL_TXS[MAX_GLOBAL_TXS] = {0};
static size_t GLOBAL_TXS_COUNT = 0;
static Resource RESOURCES[MAX_RESOURCES] = {0};
static size_t RESOURCES_COUNT = 0;
static Log STABLE_STORAGE[MAX_LOGS];
static size_t STABLE_STORAGE_COUNT = 0;

int tx_should_compare(const Tx *t1, const Tx *t2) {
  return t2->start_time < t1->start_time && t1->start_time < t2->commit_time;
}

int tx_is_conflict(const Tx *tx) {
  for (size_t i = 0; i < tx->actions_count; i++) {
    const time_t resource_write_time = tx->actions[i].rs->write_time;
    if (tx->start_time <= resource_write_time &&
        resource_write_time <= tx->commit_time) {
      return 1;
    }
  }

  return 0;
}

inline static void log_store(Log log) {
  assert(STABLE_STORAGE_COUNT + 1 <= MAX_LOGS);
  STABLE_STORAGE[STABLE_STORAGE_COUNT++] = log;
}

void tx_read(Tx *tx, Resource *rs) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  const Log log = {
      .type = READ_LOG,
      .tx_id = tx->id,
      .rs = rs,
      .time = TIME++,
  };
  log_store(log);

  const Action act = {.time = TIME++, .type = READ, .rs = rs};

  tx->actions[tx->actions_count++] = act;
}

void tx_commit(Tx *tx) {
  tx->commit_time = TIME++;
  tx->state = COMMITTED;

  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    if (tx_should_compare(tx, &GLOBAL_TXS[i])) {
      printf("Should compare %s and %s\n", tx->name, GLOBAL_TXS[i].name);
      if (tx_is_conflict(tx)) {
        // abort
      }
    }
  }
}

const char *action_t_to_string(const action_t action_t) {
  switch (action_t) {
  case READ:
    return "read";
  case WRITE:
    return "write";
  default:
    return "undefined";
  }
}

const char *log_t_to_string(const log_t log_t) {
  switch (log_t) {
  case READ_LOG:
    return "read";
  case WRITE_LOG:
    return "write";
  default:
    return "undefined";
  }
}

void tx_print(Tx *tx) {
  printf("%s\n", tx->name);
  printf("Started: %ld\n", (long)tx->start_time);
  if (tx->state >= COMMITTED)
    printf("Commited: %ld\n", (long)tx->commit_time);

  for (size_t i = 0; i < tx->actions_count; i++) {
    printf("Action %zu: \n", i);
    printf("\ttype: %s\n", action_t_to_string(tx->actions[i].type));
    printf("\tstart: %ld\n", (long)tx->actions[i].time);
    printf("\n");
  }
}

void log_print(Log *log) {
  printf("Log at time: %ld\n", log->time);
  printf("\ttype: %s\n", log_t_to_string(log->type));
  printf("\ttx id: %ld\n", log->tx_id);
  printf("\tresource: %s\n", log->rs->data);
  if (log->type == WRITE_LOG) {
    printf("\tprev: %s\n", log->prev);
    printf("\tafter: %s\n", log->after);
  }
  printf("\n");
}

void global_txs_dump(void) {
  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    tx_print(&GLOBAL_TXS[i]);
  }
}

void stable_storage_dump(void) {
  for (size_t i = 0; i < STABLE_STORAGE_COUNT; i++) {
    log_print(&STABLE_STORAGE[i]);
  }
}

inline static Tx *tx_new(char *name) {
  assert(GLOBAL_TXS_COUNT + 1 < MAX_GLOBAL_TXS);

  const size_t tx_idx = GLOBAL_TXS_COUNT;
  const time_t now = TIME;

  const Tx tx = {.state = STARTED,
                 .id = rand(),
                 .name = name,
                 .start_time = now,
                 .commit_time = now,
                 .end_time = now,
                 .actions = {0},
                 .actions_count = 0};
  GLOBAL_TXS[GLOBAL_TXS_COUNT++] = tx;
  TIME++;

  return &GLOBAL_TXS[tx_idx];
}

inline static Resource *resource_new(char *data) {
  assert(RESOURCES_COUNT + 1 < MAX_RESOURCES);

  const size_t rs_idx = RESOURCES_COUNT;
  const time_t now = TIME;

  const Resource rs = {.data = data, .write_time = now, .read_time = now};
  RESOURCES[RESOURCES_COUNT++] = rs;
  TIME++;

  return &RESOURCES[rs_idx];
}

int main(void) {
  Tx *t1 = tx_new("T1");

  Resource *r1 = resource_new("Resource A");

  tx_read(t1, r1);

  Tx *t2 = tx_new("T2");

  tx_read(t2, r1);
  tx_commit(t1);
  tx_commit(t2);

  stable_storage_dump();
  global_txs_dump();
}
