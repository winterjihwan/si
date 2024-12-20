#ifndef RECOVERY_H
#define RECOVERY_H

#define MAX_LOGS 100

#include "disk.h"
#include "tx.h"
#include <time.h>

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
  TxId tx_id;
  Resource *rs;
  char *prev;
  char *after;
} Log;

const char *recovery_log_t_to_string(const log_t log_t);
void recovery_log_print(const Log *log);
void recovery_stable_storage_dump(void);
void recovery_log_store(Log log);
void recovery_procedure_initiate(Tx *tx, time_t *time);

#endif
