MARIADB_CHARSET_INFO * STDCALL mariadb_get_charset_by_name(const char *csname)
{
  return (MARIADB_CHARSET_INFO *)mysql_find_charset_name(csname);
}