bool records_are_comparable(const TABLE *table) {
  return !table->versioned(VERS_TRX_ID) &&
          (((table->file->ha_table_flags() & HA_PARTIAL_COLUMN_READ) == 0) ||
           bitmap_is_subset(table->write_set, table->read_set));
}