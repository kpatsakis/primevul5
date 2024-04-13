maria_declare_plugin(aria)
{
  MYSQL_STORAGE_ENGINE_PLUGIN,
  &maria_storage_engine,
  "Aria",
  "MariaDB Corporation Ab",
  "Crash-safe tables with MyISAM heritage. Used for internal temporary tables and privilege tables",
  PLUGIN_LICENSE_GPL,
  ha_maria_init,                /* Plugin Init      */
  NULL,                         /* Plugin Deinit    */
  0x0105,                       /* 1.5              */
  status_variables,             /* status variables */
  system_variables,             /* system variables */
  "1.5",                        /* string version   */
  MariaDB_PLUGIN_MATURITY_STABLE /* maturity         */
}