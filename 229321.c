mysql_real_escape_string(MYSQL *mysql, char *to,const char *from,
			 ulong length)
{
  if (mysql->server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES)
    return (ulong)mysql_cset_escape_quotes(mysql->charset, to, from, length);
  else
    return (ulong)mysql_cset_escape_slashes(mysql->charset, to, from, length);
}