void STDCALL mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *cs)
{
  mariadb_get_charset_info(mysql, cs);
}