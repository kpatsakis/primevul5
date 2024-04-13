ConnectClientToUnixSockWithTimeout(const char *sockFile, unsigned int timeout)
{
#ifdef WIN32
  rfbClientErr("Windows doesn't support UNIX sockets\n");
  return RFB_INVALID_SOCKET;
#else
  rfbSocket sock;
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  if(strlen(sockFile) + 1 > sizeof(addr.sun_path)) {
      rfbClientErr("ConnectToUnixSock: socket file name too long\n");
      return RFB_INVALID_SOCKET;
  }
  strcpy(addr.sun_path, sockFile);

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock == RFB_INVALID_SOCKET) {
    rfbClientErr("ConnectToUnixSock: socket (%s)\n",strerror(errno));
    return RFB_INVALID_SOCKET;
  }

  if (!SetNonBlocking(sock))
    return RFB_INVALID_SOCKET;

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr.sun_family) + strlen(addr.sun_path)) < 0 &&
      !(errno == EINPROGRESS && WaitForConnected(sock, timeout))) {
    rfbClientErr("ConnectToUnixSock: connect\n");
    rfbCloseSocket(sock);
    return RFB_INVALID_SOCKET;
  }

  return sock;
#endif
}