static void _ma_check_print_msg(HA_CHECK *param, const char *msg_type,
                                const char *fmt, va_list args)
{
  THD *thd= (THD *) param->thd;
  Protocol *protocol= thd->protocol;
  size_t length, msg_length;
  char msgbuf[MYSQL_ERRMSG_SIZE];
  char name[NAME_LEN * 2 + 2];

  if (param->testflag & T_SUPPRESS_ERR_HANDLING)
    return;

  msg_length= my_vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
  msgbuf[sizeof(msgbuf) - 1]= 0;                // healthy paranoia

  DBUG_PRINT(msg_type, ("message: %s", msgbuf));

  if (!thd->vio_ok())
  {
    _ma_check_print(param, msg_type, msgbuf);
    return;
  }

  if (param->testflag &
      (T_CREATE_MISSING_KEYS | T_SAFE_REPAIR | T_AUTO_REPAIR))
  {
    myf flag= 0;
    if (msg_type == MA_CHECK_INFO)
      flag= ME_NOTE;
    else if (msg_type == MA_CHECK_WARNING)
      flag= ME_WARNING;
    my_message(ER_NOT_KEYFILE, msgbuf, MYF(flag));
    if (thd->variables.log_warnings > 2)
      _ma_check_print(param, msg_type, msgbuf);
    return;
  }
  length= (uint) (strxmov(name, param->db_name, ".", param->table_name,
                          NullS) - name);
  /*
    TODO: switch from protocol to push_warning here. The main reason we didn't
    it yet is parallel repair, which threads have no THD object accessible via
    current_thd.

    Also we likely need to lock mutex here (in both cases with protocol and
    push_warning).
  */
  protocol->prepare_for_resend();
  protocol->store(name, (uint)length, system_charset_info);
  protocol->store(param->op_name, system_charset_info);
  protocol->store(msg_type, system_charset_info);
  protocol->store(msgbuf, (uint)msg_length, system_charset_info);
  if (protocol->write())
    sql_print_error("Failed on my_net_write, writing to stderr instead: %s.%s: %s\n",
                    param->db_name, param->table_name, msgbuf);
  else if (thd->variables.log_warnings > 2)
    _ma_check_print(param, msg_type, msgbuf);

  return;
}