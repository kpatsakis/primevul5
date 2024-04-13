bool Locked_tables_list::restore_lock(THD *thd, TABLE_LIST *dst_table_list,
                                      TABLE *table, MYSQL_LOCK *lock)
{
  MYSQL_LOCK *merged_lock;
  DBUG_ENTER("restore_lock");
  DBUG_ASSERT(!strcmp(dst_table_list->table_name, table->s->table_name.str));

  /* Ensure we have the memory to add the table back */
  if (!(merged_lock= mysql_lock_merge(thd->lock, lock)))
    DBUG_RETURN(1);
  thd->lock= merged_lock;

  /* Link to the new table */
  dst_table_list->table= table;
  /*
    The lock type may have changed (normally it should not as create
    table will lock the table in write mode
  */
  dst_table_list->lock_type= table->reginfo.lock_type;
  table->pos_in_locked_tables= dst_table_list;

  add_back_last_deleted_lock(dst_table_list);

  table->mdl_ticket->downgrade_lock(table->reginfo.lock_type >=
                                    TL_WRITE_ALLOW_WRITE ? 
                                    MDL_SHARED_NO_READ_WRITE :
                                    MDL_SHARED_READ);

  DBUG_RETURN(0);
}