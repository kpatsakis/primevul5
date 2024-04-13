bool table_already_fk_prelocked(TABLE_LIST *tl, LEX_STRING *db,
                                LEX_STRING *table, thr_lock_type lock_type)
{
  for (; tl; tl= tl->next_global )
  {
    if (tl->lock_type >= lock_type &&
        tl->prelocking_placeholder == TABLE_LIST::FK &&
        strcmp(tl->db, db->str) == 0 &&
        strcmp(tl->table_name, table->str) == 0)
      return true;
  }
  return false;
}