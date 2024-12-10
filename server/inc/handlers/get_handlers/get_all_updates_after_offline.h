#ifndef UCHAT_GET_ALL_UPDATES_AFTER_OFFLINE_H
#define UCHAT_GET_ALL_UPDATES_AFTER_OFFLINE_H

#include "../../utils.h"
#include "../../header.h"

bool fetch_updates_from_database(PGconn *conn, int user_id, time_t timestamp, cJSON *updates);

PGresult *is_user_updated(PGconn *conn, int user_id, time_t timestamp);
PGresult *is_group_updated(PGconn *conn, int chat_id, time_t timestamp);

#endif // UCHAT_GET_ALL_UPDATES_AFTER_OFFLINE_H
