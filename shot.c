#include "assert.h"
#include "time.h"
#include <stdio.h>

#define MAX_TIME 0x7FFFFFFE
#define MAX_GLOBAL_TXS 10
#define MAX_RESOURCES 5
#define MAX_ACTIONS_COUNT 10

typedef struct {
  char *data;
  time_t write_time;
  time_t read_time;
} resource;

typedef enum { READ, WRITE } action_t;
typedef struct {
  time_t time;
  action_t type;
  resource *rs;
} action;

typedef enum { STARTED, COMMITTED, ENDED } tx_state;
typedef struct {
  char *name;
  tx_state state;

  time_t start_time;
  time_t commit_time;
  time_t end_time;

  action actions[10];
  size_t actions_count;
} tx;

static time_t TIME = 0x01;
static tx GLOBAL_TXS[MAX_GLOBAL_TXS] = {0};
static size_t GLOBAL_TXS_COUNT = 0;
static resource RESOURCES[MAX_RESOURCES] = {0};
static size_t RESOURCES_COUNT = 0;

const int tx_should_compare(const tx *t1, const tx *t2) {
  return t2->start_time < t1->start_time && t1->start_time < t2->commit_time;
}

const int tx_check_conflict(const tx *tx) {
  for (size_t i = 0; i < tx->actions_count; i++) {
    const time_t resource_write_time = tx->actions[i].rs->write_time;
    if (tx->start_time <= resource_write_time &&
        resource_write_time <= tx->commit_time) {
      return 1;
    }
  }

  return 0;
}

void tx_read(tx *tx, resource *rs) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS_COUNT);

  const action act = {.time = TIME++, .type = READ, .rs = rs};
  tx->actions[tx->actions_count++] = act;
}

void tx_commit(tx *tx) {
  tx->commit_time = TIME++;
  tx->state = COMMITTED;

  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    if (tx_should_compare(tx, &GLOBAL_TXS[i])) {
      printf("Should compare %s and %s\n", tx->name, GLOBAL_TXS[i].name);
      if (tx_check_conflict(tx)) {
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

void tx_print(tx *tx) {
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

void global_txs_dump(void) {
  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    tx_print(&GLOBAL_TXS[i]);
  }
}

inline static tx *tx_new(char *name) {
  assert(GLOBAL_TXS_COUNT + 1 < MAX_GLOBAL_TXS);

  const size_t tx_idx = GLOBAL_TXS_COUNT;
  const time_t now = TIME;

  const tx tx = {.state = STARTED,
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

inline static resource *resource_new(char *data) {
  assert(RESOURCES_COUNT + 1 < MAX_RESOURCES);

  const size_t rs_idx = RESOURCES_COUNT;
  const time_t now = TIME;

  const resource rs = {.data = data, .write_time = now, .read_time = now};
  RESOURCES[RESOURCES_COUNT++] = rs;
  TIME++;

  return &RESOURCES[rs_idx];
}

int main(void) {
  tx *t1 = tx_new("T1");

  resource *r1 = resource_new("Resource A");

  tx_read(t1, r1);

  tx *t2 = tx_new("T2");

  tx_read(t2, r1);
  tx_commit(t1);
  tx_commit(t2);

  global_txs_dump();
}
