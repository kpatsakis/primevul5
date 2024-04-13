my_bool STDCALL mysql_more_results(MYSQL *mysql)
{
  return(test(mysql->server_status & SERVER_MORE_RESULTS_EXIST));
}