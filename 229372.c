mysql_real_query(MYSQL *mysql, const char *query, unsigned long length)
{
  my_bool skip_result= OPT_EXT_VAL(mysql, multi_command);

  if (length == (unsigned long)-1)
    length= (unsigned long)strlen(query);

  free_old_query(mysql);

  if (ma_simple_command(mysql, COM_QUERY,query,length,1,0))
    return(-1);
  if (!skip_result)
    return(mysql->methods->db_read_query_result(mysql));
  return(0);
}