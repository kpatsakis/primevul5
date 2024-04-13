ConnectClientToTcpAddr(unsigned int host, int port)
{
  rfbSocket sock = ConnectClientToTcpAddrWithTimeout(host, port, DEFAULT_CONNECT_TIMEOUT);
  /* put socket back into blocking mode for compatibility reasons */
  if (sock != RFB_INVALID_SOCKET) {
    SetBlocking(sock);
  }
  return sock;
}