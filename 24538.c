void _ma_check_print_info(HA_CHECK *param, const char *fmt, ...)
{
  va_list args;
  DBUG_ENTER("_ma_check_print_info");
  va_start(args, fmt);
  _ma_check_print_msg(param, MA_CHECK_INFO, fmt, args);
  va_end(args);
  DBUG_VOID_RETURN;
}