command_error (func, code, e, flags)
     const char *func;
     int code, e, flags;	/* flags currently unused */
{
  if (code > CMDERR_LAST)
    code = CMDERR_DEFAULT;

  programming_error ("%s: %s: %d", func, _(cmd_error_table[code]), e);
}