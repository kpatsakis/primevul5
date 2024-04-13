plugin_ref plugin_lock(THD *thd, plugin_ref ptr)
{
  LEX *lex= thd ? thd->lex : 0;
  plugin_ref rc;
  DBUG_ENTER("plugin_lock");

#ifdef DBUG_OFF
  /*
    In optimized builds we don't do reference counting for built-in
    (plugin->plugin_dl == 0) plugins.

    Note that we access plugin->plugin_dl outside of LOCK_plugin, and for
    dynamic plugins a 'plugin' could correspond to plugin that was unloaded
    meanwhile!  But because st_plugin_int is always allocated on
    plugin_mem_root, the pointer can never be invalid - the memory is never
    freed.
    Of course, the memory that 'plugin' points to can be overwritten by
    another plugin being loaded, but plugin->plugin_dl can never change
    from zero to non-zero or vice versa.
    That is, it's always safe to check for plugin->plugin_dl==0 even
    without a mutex.
  */
  if (! plugin_dlib(ptr))
  {
    plugin_ref_to_int(ptr)->locks_total++;
    DBUG_RETURN(ptr);
  }
#endif
  mysql_mutex_lock(&LOCK_plugin);
  plugin_ref_to_int(ptr)->locks_total++;
  rc= intern_plugin_lock(lex, ptr);
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_RETURN(rc);
}