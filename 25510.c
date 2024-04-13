int thd_setspecific(MYSQL_THD thd, MYSQL_THD_KEY_T key, void *value)
{
  DBUG_ASSERT(key != INVALID_THD_KEY);
  if (key == INVALID_THD_KEY || (!thd && !(thd= current_thd)))
    return EINVAL;
  
  memcpy(intern_sys_var_ptr(thd, key, true), &value, sizeof(void*));
  return 0;
}