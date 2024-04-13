ma_set_connect_attrs(MYSQL *mysql, const char *host)
{
  char buffer[255];
  int rc= 0;

  rc= mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_client_name") +
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_client_version") +
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_os") +
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_server_host") +
#ifdef _WIN32
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_thread") +
#endif
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_pid") +
      mysql_options(mysql, MYSQL_OPT_CONNECT_ATTR_DELETE, "_platform");

  rc+= mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_client_name", "libmariadb")
       + mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_client_version", MARIADB_PACKAGE_VERSION)
       + mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_os", MARIADB_SYSTEM_TYPE);

  if (host && *host)
    rc+= mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_server_host", host);

#ifdef _WIN32
  snprintf(buffer, 255, "%lu", (ulong) GetCurrentThreadId());
  rc+= mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_thread", buffer);
  snprintf(buffer, 255, "%lu", (ulong) GetCurrentProcessId());
#else
  snprintf(buffer, 255, "%lu", (ulong) getpid());
#endif
  rc+= mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_pid", buffer);

  rc+= mysql_optionsv(mysql, MYSQL_OPT_CONNECT_ATTR_ADD, "_platform", MARIADB_MACHINE_TYPE);
  return(test(rc>0));
}