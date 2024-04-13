ftp_do_pasv (int csock, ip_address *addr, int *port)
{
  uerr_t err;

  /* We need to determine the address family and need to call
     getpeername, so while we're at it, store the address to ADDR.
     ftp_pasv and ftp_lpsv can simply override it.  */
  if (!socket_ip_address (csock, addr, ENDPOINT_PEER))
    abort ();

  /* If our control connection is over IPv6, then we first try EPSV and then
   * LPSV if the former is not supported. If the control connection is over
   * IPv4, we simply issue the good old PASV request. */
  switch (addr->family)
    {
    case AF_INET:
      if (!opt.server_response)
        logputs (LOG_VERBOSE, "==> PASV ... ");
      err = ftp_pasv (csock, addr, port);
      break;
    case AF_INET6:
      if (!opt.server_response)
        logputs (LOG_VERBOSE, "==> EPSV ... ");
      err = ftp_epsv (csock, addr, port);

      /* If EPSV is not supported try LPSV */
      if (err == FTPNOPASV)
        {
          if (!opt.server_response)
            logputs (LOG_VERBOSE, "==> LPSV ... ");
          err = ftp_lpsv (csock, addr, port);
        }
      break;
    default:
      abort ();
    }

  return err;
}
