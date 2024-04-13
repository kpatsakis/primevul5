FindFreeTcpPort(void)
{
  rfbSocket sock;
  int port;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == RFB_INVALID_SOCKET) {
    rfbClientErr(": FindFreeTcpPort: socket\n");
    return 0;
  }

  for (port = TUNNEL_PORT_OFFSET + 99; port > TUNNEL_PORT_OFFSET; port--) {
    addr.sin_port = htons((unsigned short)port);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
      rfbCloseSocket(sock);
      return port;
    }
  }

  rfbCloseSocket(sock);
  return 0;
}