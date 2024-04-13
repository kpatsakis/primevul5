void wsrep_plugins_post_init()
{
  THD *thd;
  I_List_iterator<THD> it(threads);

  while ((thd= it++))
  {
    if (IF_WSREP(thd->wsrep_applier,1))
    {
      // Save options_bits as it will get overwritten in plugin_thdvar_init()
      ulonglong option_bits_saved= thd->variables.option_bits;

      plugin_thdvar_init(thd);

      // Restore option_bits
      thd->variables.option_bits= option_bits_saved;
    }
  }

  return;
}