mysql_get_timeout_value(const MYSQL *mysql)
{
  unsigned int timeout= 0;

  if (mysql->options.extension && mysql->options.extension->async_context)
    timeout= mysql->options.extension->async_context->timeout_value;
  /* Avoid overflow. */
  if (timeout > UINT_MAX - 999)
    return (timeout - 1)/1000 + 1;
  else
    return (timeout+999)/1000;
}