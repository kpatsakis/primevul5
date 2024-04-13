my_bool	STDCALL mysql_change_user(MYSQL *mysql, const char *user,
				  const char *passwd, const char *db)
{
  const MARIADB_CHARSET_INFO *s_cs= mysql->charset;
  char *s_user= mysql->user,
       *s_passwd= mysql->passwd,
       *s_db= mysql->db;
  int rc;

  if (mysql->options.charset_name)
    mysql->charset= mysql_find_charset_name(mysql->options.charset_name);
  else
    mysql->charset=mysql_find_charset_name(MARIADB_DEFAULT_CHARSET);

  mysql->user= strdup(user ? user : "");
  mysql->passwd= strdup(passwd ? passwd : "");

  /* db will be set in run_plugin_auth */
  mysql->db= 0;
  rc= run_plugin_auth(mysql, 0, 0, 0, db);

  /* COM_CHANGE_USER always releases prepared statements, so we need to invalidate them */
  ma_invalidate_stmts(mysql, "mysql_change_user()");

  if (rc==0)
  {
    free(s_user);
    free(s_passwd);
    free(s_db);

    if (!mysql->db && db && !(mysql->db= strdup(db)))
    {
      SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
      rc= 1;
    }
  } else
  {
    free(mysql->user);
    free(mysql->passwd);
    free(mysql->db);

    mysql->user= s_user;
    mysql->passwd= s_passwd;
    mysql->db= s_db;
    mysql->charset= s_cs;
  }
  return(rc);
}