static void unlock_variables(THD *thd, struct system_variables *vars)
{
  intern_plugin_unlock(NULL, vars->table_plugin);
  intern_plugin_unlock(NULL, vars->tmp_table_plugin);
  intern_plugin_unlock(NULL, vars->enforced_table_plugin);
  vars->table_plugin= vars->tmp_table_plugin= vars->enforced_table_plugin= NULL;
}