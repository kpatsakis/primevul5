join_ft_read_first(JOIN_TAB *tab)
{
  int error;
  TABLE *table= tab->table;

  if (!table->file->inited &&
      (error= table->file->ha_index_init(tab->ref.key, 1)))
  {
    (void) report_error(table, error);
    return 1;
  }

  table->file->ft_init();

  if (unlikely((error= table->file->ha_ft_read(table->record[0]))))
    return report_error(table, error);
  return 0;
}