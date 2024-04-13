void* thd_getspecific(MYSQL_THD thd, MYSQL_THD_KEY_T key)
{
  DBUG_ASSERT(key != INVALID_THD_KEY);
  if (key == INVALID_THD_KEY || (!thd && !(thd= current_thd)))
    return 0;

  return *(void**)(intern_sys_var_ptr(thd, key, true));
}