mysql_select_db(MYSQL *mysql, const char *db)
{
  int error;

  if (!db)
    return 1;

  if ((error=ma_simple_command(mysql, COM_INIT_DB, db,
                               (uint) strlen(db),0,0)))
    return(error);
  free(mysql->db);
  mysql->db=strdup(db);
  return(0);
}