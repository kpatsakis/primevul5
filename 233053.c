HandshakeTLS(rfbClient* client)
{
  int timeout = 15;
  int ret;

return TRUE;

  while (timeout > 0 && (ret = SSL_do_handshake(client->tlsSession)) < 0)
  {
    if (ret != -1)
    {
      rfbClientLog("TLS handshake blocking.\n");
#ifdef WIN32
      Sleep(1000);
#else
	  sleep(1);
#endif
      timeout--;
      continue;
    }
    rfbClientLog("TLS handshake failed.\n");

    FreeTLS(client);
    return FALSE;
  }

  if (timeout <= 0)
  {
    rfbClientLog("TLS handshake timeout.\n");
    FreeTLS(client);
    return FALSE;
  }

  rfbClientLog("TLS handshake done.\n");
  return TRUE;
}