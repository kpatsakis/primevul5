void lex_unlock_plugins(LEX *lex)
{
  DBUG_ENTER("lex_unlock_plugins");

  /* release used plugins */
  if (lex->plugins.elements) /* No function call and no mutex if no plugins. */
  {
    plugin_unlock_list(0, (plugin_ref*)lex->plugins.buffer, 
                       lex->plugins.elements);
  }
  reset_dynamic(&lex->plugins);
  DBUG_VOID_RETURN;
}