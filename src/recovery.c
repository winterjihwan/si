#include "recovery.h"
#include <time.h>

static Log STABLE_STORAGE[MAX_LOGS];
static size_t STABLE_STORAGE_COUNT = 0;

const char *recovery_log_t_to_string(const log_t log_t) {
  switch (log_t) {
  case READ_LOG:
    return "read";
  case WRITE_LOG:
    return "write";
  case BEGIN_TX_LOG:
    return "begin_tx";
  case ABORT_TX_LOG:
    return "abort_tx";
  case RECOVER_LOG:
    return "recover";
  case COMMIT_TX_LOG:
    return "commit_tx";
  default:
    return "undefined";
  }
}

void recovery_log_print(const Log *log) {
  printf("Log at time: %ld\n", log->time);
  printf("\ttype: %s\n", recovery_log_t_to_string(log->type));
  printf("\ttx id: %ld\n", log->tx_id);
  if (log->type == READ_LOG || log->type == WRITE_LOG) {
    printf("\tresource: %s\n", log->rs->name);
  }
  if (log->type == WRITE_LOG) {
    printf("\tprev: %s\n", log->prev);
  }

  if (log->type == WRITE_LOG || log->type == RECOVER_LOG) {
    printf("\tafter: %s\n", log->after);
  }

  printf("\n");
}

void recovery_log_store(Log log) {
  assert(STABLE_STORAGE_COUNT + 1 <= MAX_LOGS);
  STABLE_STORAGE[STABLE_STORAGE_COUNT++] = log;
}

void recovery_stable_storage_dump(void) {
  for (size_t i = 0; i < STABLE_STORAGE_COUNT; i++) {
    recovery_log_print(&STABLE_STORAGE[i]);
  }
}

void recovery_procedure_initiate(Tx *tx, time_t *time) {
  for (size_t i = STABLE_STORAGE_COUNT - 1; i >= 0; i--) {
    if (STABLE_STORAGE[i].tx_id == tx->id) {
      const Log *log = &STABLE_STORAGE[i];
      if (log->type == BEGIN_TX_LOG) {
        break;
      }

      if (log->type == WRITE_LOG) {
        log->rs->data = log->prev;

        const Log recovery_log = {.type = RECOVER_LOG,
                                  .tx_id = tx->id,
                                  .rs = log->rs,
                                  .time = *time++,
                                  .after = log->prev};
        recovery_log_store(recovery_log);
      }
    }
  }
}
