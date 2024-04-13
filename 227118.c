ConnectClientToTcpAddrWithTimeout(unsigned int host, int port, unsigned int timeout)
{
  rfbSocket sock;
  struct sockaddr_in addr;
  int one = 1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = host;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == RFB_INVALID_SOCKET) {
#ifdef WIN32
    errno=WSAGetLastError();
#endif
    rfbClientErr("ConnectToTcpAddr: socket (%s)\n",strerror(errno));
    return RFB_INVALID_SOCKET;
  }

  if (!SetNonBlocking(sock))
    return FALSE;

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifdef WIN32
    errno=WSAGetLastError();
#endif
    if (!((errno == EWOULDBLOCK || errno == EINPROGRESS) && WaitForConnected(sock, timeout))) {
      rfbClientErr("ConnectToTcpAddr: connect\n");
      rfbCloseSocket(sock);
      return RFB_INVALID_SOCKET;
    }
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    rfbClientErr("ConnectToTcpAddr: setsockopt\n");
    rfbCloseSocket(sock);
    return RFB_INVALID_SOCKET;
  }

  return sock;
}