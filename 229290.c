end_server(MYSQL *mysql)
{
  /* if net->error 2 and reconnect is activated, we need to inforn
     connection handler */
  if (mysql->net.pvio != 0)
  {
    ma_pvio_close(mysql->net.pvio);
    mysql->net.pvio= 0;    /* Marker */
  }
  ma_net_end(&mysql->net);
  free_old_query(mysql);
  return;
}