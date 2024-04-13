mysql_ssl_set(MYSQL *mysql __attribute__((unused)),
              const char *key __attribute__((unused)),
              const char *cert __attribute__((unused)),
              const char *ca __attribute__((unused)),
              const char *capath __attribute__((unused)),
              const char *cipher __attribute__((unused)))
{
#ifdef HAVE_TLS
  char enable= 1;
  return (mysql_optionsv(mysql, MYSQL_OPT_SSL_ENFORCE, &enable) |
          mysql_optionsv(mysql, MYSQL_OPT_SSL_KEY, key) |
          mysql_optionsv(mysql, MYSQL_OPT_SSL_CERT, cert) |
          mysql_optionsv(mysql, MYSQL_OPT_SSL_CA, ca) |
          mysql_optionsv(mysql, MYSQL_OPT_SSL_CAPATH, capath) |
          mysql_optionsv(mysql, MYSQL_OPT_SSL_CIPHER, cipher)) ? 1 : 0;
#else
  return 0;
#endif
}