static void free_old_query(MYSQL *mysql)
{
  if (mysql->fields)
    ma_free_root(&mysql->field_alloc,MYF(0));
  ma_init_alloc_root(&mysql->field_alloc,8192,0);	/* Assume rowlength < 8192 */
  mysql->fields=0;
  mysql->field_count=0;				/* For API */
  mysql->info= 0;
  return;
}