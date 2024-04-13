mysql_get_ssl_cipher(MYSQL *mysql __attribute__((unused)))
{
#ifdef HAVE_TLS
  if (mysql->net.pvio && mysql->net.pvio->ctls)
  {
    return ma_pvio_tls_cipher(mysql->net.pvio->ctls);
  }
#endif
  return(NULL);
}