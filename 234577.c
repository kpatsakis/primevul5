join_read_next_same_or_null(READ_RECORD *info)
{
  int error;
  if (unlikely((error= join_read_next_same(info)) >= 0))
    return error;
  JOIN_TAB *tab= info->table->reginfo.join_tab;

  /* Test if we have already done a read after null key */
  if (*tab->ref.null_ref_key)
    return -1;					// All keys read
  *tab->ref.null_ref_key= 1;			// Set null byte
  return safe_index_read(tab);			// then read null keys
}