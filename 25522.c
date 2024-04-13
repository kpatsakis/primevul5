static void intern_plugin_unlock(LEX *lex, plugin_ref plugin)
{
  int i;
  st_plugin_int *pi;
  DBUG_ENTER("intern_plugin_unlock");

  mysql_mutex_assert_owner(&LOCK_plugin);

  if (!plugin)
    DBUG_VOID_RETURN;

  pi= plugin_ref_to_int(plugin);

#ifdef DBUG_OFF
  if (!pi->plugin_dl)
    DBUG_VOID_RETURN;
#else
  my_free(plugin);
#endif

  if (lex)
  {
    /*
      Remove one instance of this plugin from the use list.
      We are searching backwards so that plugins locked last
      could be unlocked faster - optimizing for LIFO semantics.
    */
    for (i= lex->plugins.elements - 1; i >= 0; i--)
      if (plugin == *dynamic_element(&lex->plugins, i, plugin_ref*))
      {
        delete_dynamic_element(&lex->plugins, i);
        break;
      }
    DBUG_ASSERT(i >= 0);
  }

  DBUG_ASSERT(pi->ref_count);
  pi->ref_count--;

  DBUG_PRINT("lock",("thd: %p  plugin: \"%s\" UNLOCK ref_count: %d",
                     current_thd, pi->name.str, pi->ref_count));

  if (pi->state == PLUGIN_IS_DELETED && !pi->ref_count)
    reap_needed= true;

  DBUG_VOID_RETURN;
}