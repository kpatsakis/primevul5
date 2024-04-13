ha_maria::ha_maria(handlerton *hton, TABLE_SHARE *table_arg):
handler(hton, table_arg), file(0),
int_table_flags(HA_NULL_IN_KEY | HA_CAN_FULLTEXT | HA_CAN_SQL_HANDLER |
                HA_BINLOG_ROW_CAPABLE | HA_BINLOG_STMT_CAPABLE |
                HA_DUPLICATE_POS | HA_CAN_INDEX_BLOBS | HA_AUTO_PART_KEY |
                HA_FILE_BASED | HA_CAN_GEOMETRY | TRANSACTION_STATE |
                HA_CAN_BIT_FIELD | HA_CAN_RTREEKEYS | HA_CAN_REPAIR |
                HA_CAN_VIRTUAL_COLUMNS | HA_CAN_EXPORT |
                HA_HAS_RECORDS | HA_STATS_RECORDS_IS_EXACT |
                HA_CAN_TABLES_WITHOUT_ROLLBACK),
can_enable_indexes(0), bulk_insert_single_undo(BULK_INSERT_NONE)
{}