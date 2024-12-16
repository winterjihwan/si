#include "assert.h"
#include "time.h"
#include <stdio.h>

#define MAX_TIME 0x7FFFFFFE
#define MAX_GLOBAL_TXS 10
#define MAX_ACTIONS_COUNT 10

typedef enum { read, write } action_t;
typedef struct {
  time_t time;
  action_t type;
} action;

typedef struct {
  char *tx_name;
  const time_t start_time;
  const time_t commit_time;
  const time_t end_time;
  action actions[10];
  size_t actions_count;
} tx;

static time_t TIME = 0x01;
static tx GLOBAL_TXS[10] = {0};
static size_t GLOBAL_TXS_COUNT = 0;

int tx_should_compare(const tx *t1, const tx *t2) {
  return (t1->start_time < t2->start_time) &&
         (t2->start_time < t1->commit_time);
}

void tx_read(tx *tx) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS_COUNT);
  const action act = {.time = TIME++, .type = read};
  tx->actions[tx->actions_count++] = act;
}

void tx_commit(tx *tx) {
  (void)tx;
  (void)GLOBAL_TXS;
}

const char *action_t_to_string(const action_t action_t) {
  switch (action_t) {
  case read:
    return "read";
  case write:
    return "write";
  default:
    return "undefined";
  }
}

void tx_print(tx *tx) {
  printf("%s\n", tx->tx_name);
  printf("Started: %ld\n", (long)tx->start_time);
  if (tx->commit_time)
    printf("Commited: %ld\n", (long)tx->commit_time);

  for (size_t i = 0; i < tx->actions_count; i++) {
    printf("Action %zu: \n", i);
    printf("\ttype: %s\n", action_t_to_string(tx->actions[i].type));
    printf("\tstart: %ld\n", (long)tx->actions[i].time);
    printf("\n");
  }
}

void global_txs_dump() {
  for (int i = 0; i < GLOBAL_TXS_COUNT; i++) {
    tx_print(&GLOBAL_TXS[i]);
  }
}

inline static tx tx_new(char *name) {
  assert(GLOBAL_TXS_COUNT + 1 < MAX_GLOBAL_TXS);
  const tx tx = {.tx_name = name,
                 .start_time = TIME++,
                 .commit_time = MAX_TIME,
                 .end_time = MAX_TIME,
                 .actions = {0},
                 .actions_count = 0};

  return tx;
}

int main(void) {
  tx t1 = tx_new("T1");

  tx_read(&t1);

  tx t2 = tx_new("T2");

  tx_read(&t2);

  global_txs_dump();
}
