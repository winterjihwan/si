#include "tx.h"
#include "assert.h"
#include "disk.h"
#include "recovery.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>

static time_t TIME = 0x01;
static Tx GLOBAL_TXS[MAX_GLOBAL_TXS] = {0};
static size_t GLOBAL_TXS_COUNT = 0;
static Resource RESOURCES[MAX_RESOURCES] = {0};
static size_t RESOURCES_COUNT = 0;

int tx_should_compare(const Tx *t1, const Tx *t2) {
  return t2->start_time < t1->start_time && t1->start_time < t2->commit_time;
}

int tx_is_conflict(const Tx *tx) {
  for (size_t i = 0; i < tx->actions_count; i++) {
    if (tx->actions[i].type != WRITE) {
      continue;
    }

    const time_t resource_write_time = tx->actions[i].rs->version;
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

void rs_print(Resource *rs) {
  printf("Resource %s\n", rs->name);
  printf("\tlast write: %ld\n", (long)rs->version);
  printf("\tdata %s\n", rs->data);
}

void tx_read(Tx *tx, Table *table, char *rs_name) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  Resource rs = *disk_table_read(table, rs_name);

  const Log log = {
      .type = READ_LOG,
      .tx_id = tx->id,
      .rs = &rs,
      .time = TIME++,
  };
  recovery_log_store(log);

  const Action act = {.time = TIME++, .type = READ, .rs = &rs};

  tx->actions[tx->actions_count++] = act;
}

void tx_write(Tx *tx, Table *table, char *rs_name, char *new_data) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  Resource rs = *disk_table_read(table, rs_name);

  const Log log = {.type = WRITE_LOG,
                   .tx_id = tx->id,
                   .rs = &rs,
                   .time = TIME++,
                   .prev = rs.data,
                   .after = new_data};
  recovery_log_store(log);

  const Action act = {.time = TIME++, .type = WRITE, .rs = &rs};
  tx->actions[tx->actions_count++] = act;
}

void tx_abort(Tx *tx) {
  assert(tx->actions_count + 1 <= MAX_ACTIONS);

  const Log log = {
      .type = ABORT_TX_LOG,
      .tx_id = tx->id,
      .time = TIME++,
  };
  recovery_log_store(log);

  recovery_procedure_initiate(tx, &TIME);
}

void tx_commit(Tx *tx) {
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

  for (size_t i = 0; i < tx->actions_count; i++) {
    if (tx->actions[i].type == WRITE) {
      tx->actions[i].rs->version = tx->commit_time;
    }
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

void resources_dump(void) {
  for (size_t i = 0; i < RESOURCES_COUNT; i++) {
    rs_print(&RESOURCES[i]);
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
               action_t_to_string(act.type), act.rs->name);
      };
    }

    for (size_t j = 0; j < t2->actions_count; j++) {
      if (t2->actions[j].time == (time_t)i) {
        const Action act = t2->actions[j];
        printf("%4zu|              |%11s(%s) \n", i,
               action_t_to_string(act.type), act.rs->name);
      };
    }
  };

  printf("\n");
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

  const Log log = {
      .type = BEGIN_TX_LOG,
      .tx_id = tx.id,
      .time = TIME++,
  };
  recovery_log_store(log);

  return &GLOBAL_TXS[tx_idx];
}

static Disk DISK = {0};

int main(void) {
  //-Setup-//
  Resource r1 = resource_new(TIME++, "X", "Hi goblin!");
  Table *tableA = disk_table_new(&DISK, "Table A");
  disk_table_insert(tableA, r1);
  //------//

  Tx *t1 = tx_new("T1");

  tx_read(t1, tableA, "X");
  tx_write(t1, tableA, "X", "Hi angel!");

  disk_table_dump(tableA);

  Tx *t2 = tx_new("T2");

  tx_commit(t1);

  tx_write(t2, tableA, "X", "Welcome to Seoul");

  disk_table_dump(tableA);

  tx_read(t2, tableA, "X");
  tx_commit(t2);

  /*resources_dump();*/
  /*tx_schedule_dump(t1, t2);*/
  /*stable_storage_dump();*/
  /*global_txs_dump();*/
}
