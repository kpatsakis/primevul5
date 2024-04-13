ConnectClientToTcpAddr6(const char *hostname, int port)
{
  rfbSocket sock = ConnectClientToTcpAddr6WithTimeout(hostname, port, DEFAULT_CONNECT_TIMEOUT);
  /* put socket back into blocking mode for compatibility reasons */
  if (sock != RFB_INVALID_SOCKET) {
    SetBlocking(sock);
  }
  return sock;
}