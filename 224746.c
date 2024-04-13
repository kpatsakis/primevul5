command_errstr (code)
     int code;
{
  if (code > CMDERR_LAST)
    code = CMDERR_DEFAULT;

  return (_(cmd_error_table[code]));
}