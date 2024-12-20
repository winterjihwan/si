#include "tx.h"
#include "assert.h"
#include "disk.h"
#include "recovery.h"
#include "snapshot.h"
#include "time.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static Tx GLOBAL_TXS[MAX_GLOBAL_TXS] = {0};
static size_t GLOBAL_TXS_COUNT = 0;

extern time_t TIME;
extern Disk DISK;

int tx_should_compare(const Tx *t1, const Tx *t2) {
  return t2->start_time < t1->start_time && t1->start_time < t2->commit_time;
}

int tx_is_conflict(const Tx *tx) {
  for (size_t i = 0; i < tx->actions_count; i++) {
    if (tx->actions[i].type != WRITE) {
      continue;
    }

    const time_t resource_write_time = tx->actions[i].time;
    if (tx->start_time <= resource_write_time &&
        resource_write_time <= tx->commit_time) {
      return 1;
    }
  }

  return 0;
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

void tx_print(Tx *tx) {
  printf("%s\n", tx->name);
  printf("Started: %ld\n", (long)tx->start_time);
  if (tx->state >= COMMITTED)
    printf("Commited: %ld\n", (long)tx->commit_time);

  for (size_t i = 0; i < tx->actions_count; i++) {
    printf("Action %zu: \n", i);
    printf("\ttype: %s\n", action_t_to_string(tx->actions[i].type));
    printf("\time: %ld\n", (long)tx->actions[i].time);
    printf("\n");
  }
}

void tx_read(Tx *tx, char *rs_name) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  Resource rs = *disk_table_read(&tx->workspace, rs_name);

  const Log log = {
      .type = READ_LOG,
      .tx_id = tx->id,
      .rs = &rs,
      .time = TIME++,
  };
  recovery_log_store(log);

  const Action act = {
      .time = TIME++, .type = READ, .data_name = rs.name, .data_cur = rs};

  tx->actions[tx->actions_count++] = act;
}

void tx_write(Tx *tx, char *rs_name, char *new_data) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  Resource rs = *disk_table_read(&tx->workspace, rs_name);

  const Log log = {.type = WRITE_LOG,
                   .tx_id = tx->id,
                   .rs = &rs,
                   .time = TIME++,
                   .prev = rs.data,
                   .after = new_data};
  recovery_log_store(log);

  rs.data = new_data;
  disk_table_write(&tx->workspace, rs_name, (void *)&rs);

  const Action act = {
      .time = TIME++, .type = WRITE, .data_name = rs.name, .data_cur = rs};
  tx->actions[tx->actions_count++] = act;
}

void tx_abort(Tx *tx) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);
  fprintf(stderr, "ABORT: Transaction %s aborted \n", tx->name);

  const Log log = {
      .type = ABORT_TX_LOG,
      .tx_id = tx->id,
      .time = TIME++,
  };
  recovery_log_store(log);

  recovery_procedure_initiate(tx, &TIME);
}

void tx_commit(Tx *tx, char *table_name) {
  assert(tx->state != COMMITTED | tx->state != ABORTED);
  tx->commit_time = TIME++;
  tx->state = COMMITTED;

  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    if (tx_should_compare(tx, &GLOBAL_TXS[i])) {
      if (tx_is_conflict(tx)) {
        tx_abort(tx);
        return;
      }
    }
  }

  Table *table = disk_table(&DISK, table_name);

  for (size_t i = 0; i < tx->actions_count; i++) {
    Action *action = &tx->actions[i];

    if (action->type == WRITE) {
      action->time = tx->commit_time;
    }

    disk_table_write(table, action->data_name, (void *)&action->data_cur);
  }

  const Log log = {
      .type = COMMIT_TX_LOG,
      .tx_id = tx->id,
      .time = TIME++,
  };
  recovery_log_store(log);
}

void global_txs_dump(void) {
  for (size_t i = 0; i < GLOBAL_TXS_COUNT; i++) {
    tx_print(&GLOBAL_TXS[i]);
  }
}

void tx_schedule_dump(const Tx *t1, const Tx *t2) {
  size_t t1_last_el_idx = t1->actions_count - 1;
  size_t t2_last_el_idx = t2->actions_count - 1;

  size_t max_time =
      MAX(t1->actions[t1_last_el_idx].time, t2->actions[t2_last_el_idx].time);

  printf("t   |      %s      |        %s        \n", t1->name, t2->name);
  printf("--------------------------------------\n");
  for (size_t i = 0; i <= max_time; i++) {
    for (size_t j = 0; j < t1->actions_count; j++) {
      if (t1->actions[j].time == (time_t)i) {
        const Action act = t1->actions[j];
        printf("%4zu|%11s(%s)|              \n", i,
               action_t_to_string(act.type), act.data_name);
      };
    }

    for (size_t j = 0; j < t2->actions_count; j++) {
      if (t2->actions[j].time == (time_t)i) {
        const Action act = t2->actions[j];
        printf("%4zu|              |%11s(%s) \n", i,
               action_t_to_string(act.type), act.data_name);
      };
    }
  };

  printf("\n");
}

// initialize variables to own workspace
Tx *tx_new(char *name, Table *table, char **names, size_t names_size) {
  assert(GLOBAL_TXS_COUNT + 1 < MAX_GLOBAL_TXS);

  const size_t tx_idx = GLOBAL_TXS_COUNT;
  const time_t now = TIME;

  const Snapshot snapshot = snapshot_initiate(&TIME, table, names, names_size);

  const Tx tx = {.state = STARTED,
                 .id = rand(),
                 .name = name,
                 .start_time = now,
                 .commit_time = now,
                 .end_time = now,
                 .actions = {0},
                 .actions_count = 0,
                 .workspace = snapshot.table};
  GLOBAL_TXS[GLOBAL_TXS_COUNT++] = tx;
  TIME++;

  const Log log = {
      .type = BEGIN_TX_LOG,
      .tx_id = tx.id,
      .time = TIME++,
  };
  recovery_log_store(log);

  return &GLOBAL_TXS[tx_idx];
}
