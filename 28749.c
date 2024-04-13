ftp_do_port (int csock, int *local_sock)
{
  uerr_t err;
  ip_address cip;

  if (!socket_ip_address (csock, &cip, ENDPOINT_PEER))
    abort ();

  /* If our control connection is over IPv6, then we first try EPRT and then
   * LPRT if the former is not supported. If the control connection is over
   * IPv4, we simply issue the good old PORT request. */
  switch (cip.family)
    {
    case AF_INET:
      if (!opt.server_response)
        logputs (LOG_VERBOSE, "==> PORT ... ");
      err = ftp_port (csock, local_sock);
      break;
    case AF_INET6:
      if (!opt.server_response)
        logputs (LOG_VERBOSE, "==> EPRT ... ");
      err = ftp_eprt (csock, local_sock);

      /* If EPRT is not supported try LPRT */
      if (err == FTPPORTERR)
        {
          if (!opt.server_response)
            logputs (LOG_VERBOSE, "==> LPRT ... ");
          err = ftp_lprt (csock, local_sock);
        }
      break;
    default:
      abort ();
    }
  return err;
}
