ConnectClientToUnixSock(const char *sockFile)
{
  rfbSocket sock = ConnectClientToUnixSockWithTimeout(sockFile, DEFAULT_CONNECT_TIMEOUT);
  /* put socket back into blocking mode for compatibility reasons */
  if (sock != RFB_INVALID_SOCKET) {
    SetBlocking(sock);
  }
  return sock;
}