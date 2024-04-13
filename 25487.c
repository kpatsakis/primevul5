static void report_error(int where_to, uint error, ...)
{
  va_list args;
  DBUG_ASSERT(where_to & (REPORT_TO_USER | REPORT_TO_LOG));
  if (where_to & REPORT_TO_USER)
  {
    va_start(args, error);
    my_printv_error(error, ER(error), MYF(0), args);
    va_end(args);
  }
  if (where_to & REPORT_TO_LOG)
  {
    va_start(args, error);
    error_log_print(ERROR_LEVEL, ER_DEFAULT(error), args);
    va_end(args);
  }
}