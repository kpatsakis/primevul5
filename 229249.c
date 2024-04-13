void my_set_error(MYSQL *mysql,
                  unsigned int error_nr,
                  const char *sqlstate,
                  const char *format,
                  ...)
{
  va_list ap;

  const char *errmsg;

  if (!format)
  {
    if (error_nr >= CR_MIN_ERROR && error_nr <= CR_MYSQL_LAST_ERROR)
      errmsg= ER(error_nr);
    else if (error_nr >= CER_MIN_ERROR && error_nr <= CR_MARIADB_LAST_ERROR)
      errmsg= CER(error_nr);
    else
      errmsg= ER(CR_UNKNOWN_ERROR);
  }

  mysql->net.last_errno= error_nr;
  ma_strmake(mysql->net.sqlstate, sqlstate, SQLSTATE_LENGTH);
  va_start(ap, format);
  vsnprintf(mysql->net.last_error, MYSQL_ERRMSG_SIZE - 1,
            format ? format : errmsg, ap);
  va_end(ap);
  return;
}