void mthd_my_skip_result(MYSQL *mysql)
{
  ulong pkt_len;

  do {
    pkt_len= ma_net_safe_read(mysql);
    if (pkt_len == packet_error)
      break;
  } while (pkt_len > 8 || mysql->net.read_pos[0] != 254);
  return;
}