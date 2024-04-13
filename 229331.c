MARIADB_CHARSET_INFO * STDCALL mariadb_get_charset_by_nr(unsigned int csnr)
{
  return (MARIADB_CHARSET_INFO *)mysql_find_charset_nr(csnr);
}