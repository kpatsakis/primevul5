mysql_get_timeout_value_ms(const MYSQL *mysql)
{
  if (mysql->options.extension && mysql->options.extension->async_context)
    return mysql->options.extension->async_context->timeout_value;
  return 0;
}