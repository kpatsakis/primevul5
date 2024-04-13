ma_net_safe_read(MYSQL *mysql)
{
  NET *net= &mysql->net;
  ulong len=0;

restart:
  if (net->pvio != 0)
    len=ma_net_read(net);

  if (len == packet_error || len == 0)
  {
    end_server(mysql);
    my_set_error(mysql, net->last_errno == ER_NET_PACKET_TOO_LARGE ?
		     CR_NET_PACKET_TOO_LARGE:
		     CR_SERVER_LOST,
         SQLSTATE_UNKNOWN, 0, errno);
    return(packet_error);
  }
  if (net->read_pos[0] == 255)
  {
    if (len > 3)
    {
      char *pos=(char*) net->read_pos+1;
      uint last_errno=uint2korr(pos);
      pos+=2;
      len-=2;

      if (last_errno== 65535 &&
          ((mariadb_connection(mysql) && (mysql->server_capabilities & CLIENT_PROGRESS)) ||
           (!(mysql->extension->mariadb_server_capabilities & MARIADB_CLIENT_PROGRESS << 32))))
      {
        if (cli_report_progress(mysql, (uchar *)pos, (uint) (len-1)))
        {
          /* Wrong packet */
          my_set_error(mysql, CR_MALFORMED_PACKET, SQLSTATE_UNKNOWN, 0);
          return (packet_error);
        }
        goto restart;
      }
      net->last_errno= last_errno;
      if (pos[0]== '#')
      {
        ma_strmake(net->sqlstate, pos+1, SQLSTATE_LENGTH);
        pos+= SQLSTATE_LENGTH + 1;
      }
      else
      {
        strncpy(net->sqlstate, SQLSTATE_UNKNOWN, SQLSTATE_LENGTH);
      }
      ma_strmake(net->last_error,(char*) pos,
              min(len,sizeof(net->last_error)-1));
    }
    else
    {
      my_set_error(mysql, CR_UNKNOWN_ERROR, SQLSTATE_UNKNOWN, 0);
    }

    mysql->server_status&= ~SERVER_MORE_RESULTS_EXIST;

    return(packet_error);
  }
  return len;
}