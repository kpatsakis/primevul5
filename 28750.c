ftp_do_port (int csock, int *local_sock)
{
  if (!opt.server_response)
    logputs (LOG_VERBOSE, "==> PORT ... ");
  return ftp_port (csock, local_sock);
}
