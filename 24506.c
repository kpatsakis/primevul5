void _ma_check_print_error(HA_CHECK *param, const char *fmt, ...)
{
  va_list args;
  DBUG_ENTER("_ma_check_print_error");
  param->error_printed++;
  param->out_flag |= O_DATA_LOST;
  if (param->testflag & T_SUPPRESS_ERR_HANDLING)
    DBUG_VOID_RETURN;
  va_start(args, fmt);
  _ma_check_print_msg(param, MA_CHECK_ERROR, fmt, args);
  va_end(args);
  DBUG_VOID_RETURN;
}