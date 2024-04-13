static my_bool close_cached_tables_callback(TDC_element *element,
                                            close_cached_tables_arg *arg)
{
  mysql_mutex_lock(&element->LOCK_table_share);
  if (element->share && element->flushed &&
      element->version < arg->refresh_version)
  {
    /* wait_for_old_version() will unlock mutex and free share */
    arg->element= element;
    return TRUE;
  }
  mysql_mutex_unlock(&element->LOCK_table_share);
  return FALSE;
}