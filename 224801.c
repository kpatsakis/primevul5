report_error (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  va_list args;

  error_prolog (1);

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");

  va_end (args);
  if (exit_immediately_on_error)
    {
      if (last_command_exit_value == 0)
	last_command_exit_value = 1;
      exit_shell (last_command_exit_value);
    }
}