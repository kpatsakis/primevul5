static uchar *intern_sys_var_ptr(THD* thd, int offset, bool global_lock)
{
  DBUG_ENTER("intern_sys_var_ptr");
  DBUG_ASSERT(offset >= 0);
  DBUG_ASSERT((uint)offset <= global_system_variables.dynamic_variables_head);

  if (!thd)
    DBUG_RETURN((uchar*) global_system_variables.dynamic_variables_ptr + offset);

  /*
    dynamic_variables_head points to the largest valid offset
  */
  if (!thd->variables.dynamic_variables_ptr ||
      (uint)offset > thd->variables.dynamic_variables_head)
  {
    mysql_prlock_rdlock(&LOCK_system_variables_hash);
    sync_dynamic_session_variables(thd, global_lock);
    mysql_prlock_unlock(&LOCK_system_variables_hash);
  }
  DBUG_RETURN((uchar*)thd->variables.dynamic_variables_ptr + offset);
}