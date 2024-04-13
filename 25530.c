static int pluginvar_sysvar_flags(const st_mysql_sys_var *p)
{
  return (p->flags & PLUGIN_VAR_THDLOCAL ? sys_var::SESSION : sys_var::GLOBAL)
       | (p->flags & PLUGIN_VAR_READONLY ? sys_var::READONLY : 0);
}