int thd_key_create(MYSQL_THD_KEY_T *key)
{
  int flags= PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_STR |
             PLUGIN_VAR_NOSYSVAR | PLUGIN_VAR_NOCMDOPT;
  char namebuf[256];
  snprintf(namebuf, sizeof(namebuf), "%u", thd_key_no++);
  mysql_prlock_wrlock(&LOCK_system_variables_hash);
  // non-letters in the name as an extra safety
  st_bookmark *bookmark= register_var("\a\v\a\t\a\r", namebuf, flags);
  mysql_prlock_unlock(&LOCK_system_variables_hash);
  if (bookmark)
  {
    *key= bookmark->offset;
    return 0;
  }
  return ENOMEM;
}