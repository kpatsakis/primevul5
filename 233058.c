ReadFromTLS(rfbClient* client, char *out, unsigned int n)
{
  ssize_t ret;

  ret = SSL_read (client->tlsSession, out, n);

  if (ret >= 0)
    return ret;
  else {
    errno = ssl_errno (client->tlsSession, ret);

    if (errno != EAGAIN) {
      rfbClientLog("Error reading from TLS: -.\n");
    }
  }

  return -1;
}