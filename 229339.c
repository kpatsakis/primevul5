int STDCALL mysql_set_character_set(MYSQL *mysql, const char *csname)
{
  const MARIADB_CHARSET_INFO *cs;

  if (!csname)
    goto error;

  if ((cs= mysql_find_charset_name(csname)))
  {
    char buff[64];

    snprintf(buff, 63, "SET NAMES %s", cs->csname);
    if (!mysql_real_query(mysql, buff, (unsigned long)strlen(buff)))
    {
      mysql->charset= cs;
      return(0);
    }
    return(mysql->net.last_errno);
  }

error:
  my_set_error(mysql, CR_CANT_READ_CHARSET, SQLSTATE_UNKNOWN,
               0, csname, "compiled_in");
  return(mysql->net.last_errno);
}