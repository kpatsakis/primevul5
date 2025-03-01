HandleAnonTLSAuth(rfbClient* client)
{
  if (!InitializeTLS() || !InitializeTLSSession(client, TRUE, NULL)) return FALSE;

  if (!HandshakeTLS(client)) return FALSE;

  return TRUE;
}