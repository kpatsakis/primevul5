unique_table(THD *thd, TABLE_LIST *table, TABLE_LIST *table_list,
             uint check_flag)
{
  TABLE_LIST *dup;

  table= table->find_table_for_update();

  if (table->table && table->table->file->ht->db_type == DB_TYPE_MRG_MYISAM)
  {
    TABLE_LIST *child;
    dup= NULL;
    /* Check duplicates of all merge children. */
    for (child= table->next_global; child && child->parent_l == table;
         child= child->next_global)
    {
      if ((dup= find_dup_table(thd, child, child->next_global, check_flag)))
        break;
    }
  }
  else
    dup= find_dup_table(thd, table, table_list, check_flag);
  return dup;
}