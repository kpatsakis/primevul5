static void update_maria_group_commit_interval(MYSQL_THD thd,
                                               struct st_mysql_sys_var *var,
                                               void *var_ptr, const void *save)
{
  ulong new_value= (ulong)*((long *)save);
  ulong *value_ptr= (ulong*) var_ptr;
  DBUG_ENTER("update_maria_group_commit_interval");
  DBUG_PRINT("enter", ("old value: %lu  new value %lu  group commit %lu",
                        *value_ptr, new_value, maria_group_commit));

  /* variable change made under global lock so we can just read it */
  switch (maria_group_commit) {
    case TRANSLOG_GCOMMIT_NONE:
      *value_ptr= new_value;
      translog_set_group_commit_interval(new_value);
      break;
    case TRANSLOG_GCOMMIT_HARD:
      *value_ptr= new_value;
      translog_set_group_commit_interval(new_value);
      break;
    case TRANSLOG_GCOMMIT_SOFT:
      if (*value_ptr)
        translog_soft_sync_end();
      translog_set_group_commit_interval(new_value);
      if ((*value_ptr= new_value))
        translog_soft_sync_start();
      break;
    default:
      DBUG_ASSERT(0); /* impossible */
  }
  DBUG_VOID_RETURN;
}