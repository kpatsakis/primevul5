int read_first_record_seq(JOIN_TAB *tab)
{
  if (tab->read_record.table->file->ha_rnd_init_with_error(1))
    return 1;
  return (*tab->read_record.read_record)(&tab->read_record);
}