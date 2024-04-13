void rr_unlock_row(st_join_table *tab)
{
  READ_RECORD *info= &tab->read_record;
  info->table->file->unlock_row();
}