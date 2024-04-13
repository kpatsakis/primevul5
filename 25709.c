bool open_tmp_table(TABLE *table)
{
  int error;
  if ((error= table->file->ha_open(table, table->s->path.str, O_RDWR,
                                   HA_OPEN_TMP_TABLE |
                                   HA_OPEN_INTERNAL_TABLE)))
  {
    table->file->print_error(error, MYF(0)); /* purecov: inspected */
    table->db_stat= 0;
    return 1;
  }
  table->db_stat= HA_OPEN_KEYFILE;
  (void) table->file->extra(HA_EXTRA_QUICK); /* Faster */
  if (!table->is_created())
  {
    table->set_created();
    table->in_use->inc_status_created_tmp_tables();
  }

  return 0;
}