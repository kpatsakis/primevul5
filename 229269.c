int STDCALL mariadb_cancel(MYSQL *mysql)
{
  if (!mysql || !mysql->net.pvio || !mysql->net.pvio->methods || !mysql->net.pvio->methods->shutdown)
  {
    return 1;
  }
  else
  {
    MARIADB_PVIO *pvio = mysql->net.pvio;
    return pvio->methods->shutdown(pvio);
  }
}