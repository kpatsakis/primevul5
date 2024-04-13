static void mariadb_get_charset_info(MYSQL *mysql, MY_CHARSET_INFO *cs)
{
  if (!cs)
    return;

  cs->number= mysql->charset->nr;
  cs->csname=  mysql->charset->csname;
  cs->name= mysql->charset->name;
  cs->state= 0;
  cs->comment= NULL;
  cs->dir= NULL;
  cs->mbminlen= mysql->charset->char_minlen;
  cs->mbmaxlen= mysql->charset->char_maxlen;

  return;
}