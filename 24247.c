bool wait_while_table_is_used(THD *thd, TABLE *table,
                              enum ha_extra_function function)
{
  DBUG_ENTER("wait_while_table_is_used");
  DBUG_ASSERT(!table->s->tmp_table);
  DBUG_PRINT("enter", ("table: '%s'  share: %p  db_stat: %u  version: %lld",
                       table->s->table_name.str, table->s,
                       table->db_stat, table->s->tdc->version));

  if (thd->mdl_context.upgrade_shared_lock(
             table->mdl_ticket, MDL_EXCLUSIVE,
             thd->variables.lock_wait_timeout))
    DBUG_RETURN(TRUE);

  tdc_remove_table(thd, TDC_RT_REMOVE_NOT_OWN,
                   table->s->db.str, table->s->table_name.str,
                   FALSE);
  /* extra() call must come only after all instances above are closed */
  if (function != HA_EXTRA_NOT_USED)
    DBUG_RETURN(table->file->extra(function));
  DBUG_RETURN(FALSE);
}