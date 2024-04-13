ftp_do_pasv (int csock, ip_address *addr, int *port)
{
  if (!opt.server_response)
    logputs (LOG_VERBOSE, "==> PASV ... ");
  return ftp_pasv (csock, addr, port);
}
