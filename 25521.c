sys_var_pluginvar::sys_var_pluginvar(sys_var_chain *chain, const char *name_arg,
        st_plugin_int *p, st_mysql_sys_var *pv)
    : sys_var(chain, name_arg, pv->comment, pluginvar_sysvar_flags(pv),
              0, pv->flags & PLUGIN_VAR_NOCMDOPT ? -1 : 0, NO_ARG,
              pluginvar_show_type(pv), 0,
              NULL, VARIABLE_NOT_IN_BINLOG, NULL, NULL, NULL),
    plugin(p), plugin_var(pv)
{
  plugin_var->name= name_arg;
  plugin_opt_set_limits(&option, pv);
}