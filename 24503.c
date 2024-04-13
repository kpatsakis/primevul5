void _ma_check_print_warning(HA_CHECK *param, const char *fmt, ...)
{
  va_list args;
  DBUG_ENTER("_ma_check_print_warning");
  param->warning_printed++;
  param->out_flag |= O_DATA_LOST;
  va_start(args, fmt);
  _ma_check_print_msg(param, MA_CHECK_WARNING, fmt, args);
  va_end(args);
  DBUG_VOID_RETURN;
}