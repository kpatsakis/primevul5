ConnectClientToTcpAddr6WithTimeout(const char *hostname, int port, unsigned int timeout)
{
#ifdef LIBVNCSERVER_IPv6
  rfbSocket sock;
  int n;
  struct addrinfo hints, *res, *ressave;
  char port_s[10];
  int one = 1;

  snprintf(port_s, 10, "%d", port);
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((n = getaddrinfo(strcmp(hostname,"") == 0 ? "localhost": hostname, port_s, &hints, &res)))
  {
    rfbClientErr("ConnectClientToTcpAddr6: getaddrinfo (%s)\n", gai_strerror(n));
    return RFB_INVALID_SOCKET;
  }

  ressave = res;
  sock = RFB_INVALID_SOCKET;
  while (res)
  {
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock != RFB_INVALID_SOCKET)
    {
      if (SetNonBlocking(sock)) {
        if (connect(sock, res->ai_addr, res->ai_addrlen) == 0) {
          break;
        } else {
#ifdef WIN32
          errno=WSAGetLastError();
#endif
          if ((errno == EWOULDBLOCK || errno == EINPROGRESS) && WaitForConnected(sock, timeout))
            break;
          rfbCloseSocket(sock);
          sock = RFB_INVALID_SOCKET;
        }
      } else {
        rfbCloseSocket(sock);
        sock = RFB_INVALID_SOCKET;
      }
    }
    res = res->ai_next;
  }
  freeaddrinfo(ressave);

  if (sock == RFB_INVALID_SOCKET)
  {
    rfbClientErr("ConnectClientToTcpAddr6: connect\n");
    return RFB_INVALID_SOCKET;
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    rfbClientErr("ConnectToTcpAddr: setsockopt\n");
    rfbCloseSocket(sock);
    return RFB_INVALID_SOCKET;
  }

  return sock;

#else

  rfbClientErr("ConnectClientToTcpAddr6: IPv6 disabled\n");
  return RFB_INVALID_SOCKET;

#endif
}