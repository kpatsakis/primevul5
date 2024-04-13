static void mysql_close_memory(MYSQL *mysql)
{
  ma_clear_session_state(mysql);
  free(mysql->host_info);
  free(mysql->host);
  free(mysql->user);
  free(mysql->passwd);
  free(mysql->db);
  free(mysql->unix_socket);
  free(mysql->server_version);
  mysql->host_info= mysql->host= mysql->unix_socket=
                    mysql->server_version=mysql->user=mysql->passwd=mysql->db=0;
}