AcceptTcpConnection(rfbSocket listenSock)
{
  rfbSocket sock;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  int one = 1;

  sock = accept(listenSock, (struct sockaddr *) &addr, &addrlen);
  if (sock == RFB_INVALID_SOCKET) {
    rfbClientErr("AcceptTcpConnection: accept\n");
    return RFB_INVALID_SOCKET;
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    rfbClientErr("AcceptTcpConnection: setsockopt\n");
    rfbCloseSocket(sock);
    return RFB_INVALID_SOCKET;
  }

  return sock;
}