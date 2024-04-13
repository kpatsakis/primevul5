static void _ma_check_print(HA_CHECK *param, const char* msg_type,
                            const char *msgbuf)
{
  if (msg_type == MA_CHECK_INFO)
    sql_print_information("%s.%s: %s", param->db_name, param->table_name,
                          msgbuf);
  else if (msg_type == MA_CHECK_WARNING)
    sql_print_warning("%s.%s: %s", param->db_name, param->table_name,
                      msgbuf);
  else
    sql_print_error("%s.%s: %s", param->db_name, param->table_name, msgbuf);
}