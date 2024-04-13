WriteToTLS(rfbClient* client, const char *buf, unsigned int n)
{
  unsigned int offset = 0;
  ssize_t ret;

  while (offset < n)
  {

    ret = SSL_write (client->tlsSession, buf + offset, (size_t)(n-offset));

    if (ret < 0)
      errno = ssl_errno (client->tlsSession, ret);

    if (ret == 0) continue;
    if (ret < 0)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
      rfbClientLog("Error writing to TLS: -\n");
      return -1;
    }
    offset += (unsigned int)ret;
  }
  return offset;
}