InitializeTLSSession(rfbClient* client, rfbBool anonTLS, rfbCredential *cred)
{
  if (client->tlsSession) return TRUE;

  client->tlsSession = open_ssl_connection (client, client->sock, anonTLS, cred);

  if (!client->tlsSession)
    return FALSE;

  rfbClientLog("TLS session initialized.\n");

  return TRUE;
}