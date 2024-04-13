ma_simple_command(MYSQL *mysql,enum enum_server_command command, const char *arg,
	       size_t length, my_bool skipp_check, void *opt_arg)
{
  if ((mysql->options.client_flag & CLIENT_LOCAL_FILES) &&
       mysql->options.extension && mysql->extension->auto_local_infile == WAIT_FOR_QUERY &&
       arg && (*arg == 'l' || *arg == 'L') &&
       command == COM_QUERY)
  {
    if (strncasecmp(arg, "load", 4) == 0)
      mysql->extension->auto_local_infile= ACCEPT_FILE_REQUEST;
  }
  return mysql->methods->db_command(mysql, command, arg, length, skipp_check, opt_arg);
}