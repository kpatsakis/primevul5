static rfbBool WaitForConnected(int socket, unsigned int secs)
{
  fd_set writefds;
  fd_set exceptfds;
  struct timeval timeout;

  timeout.tv_sec=secs;
  timeout.tv_usec=0;

  FD_ZERO(&writefds);
  FD_SET(socket, &writefds);
  FD_ZERO(&exceptfds);
  FD_SET(socket, &exceptfds);
  if (select(socket+1, NULL, &writefds, &exceptfds, &timeout)==1) {
#ifdef WIN32
    if (FD_ISSET(socket, &exceptfds))
      return FALSE;
#else
    int so_error;
    socklen_t len = sizeof so_error;
    getsockopt(socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error!=0)
      return FALSE;
#endif
    return TRUE;
  }

  return FALSE;
}