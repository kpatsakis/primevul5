HandleVeNCryptAuth(rfbClient* client)
{
  uint8_t major, minor, status;
  uint32_t authScheme;
  rfbBool anonTLS;
  rfbCredential *cred = NULL;
  rfbBool result = TRUE;

  if (!InitializeTLS()) return FALSE;

  /* Read VeNCrypt version */
  if (!ReadFromRFBServer(client, (char *)&major, 1) ||
      !ReadFromRFBServer(client, (char *)&minor, 1))
  {
    return FALSE;
  }
  rfbClientLog("Got VeNCrypt version %d.%d from server.\n", (int)major, (int)minor);

  if (major != 0 && minor != 2)
  {
    rfbClientLog("Unsupported VeNCrypt version.\n");
    return FALSE;
  }

  if (!WriteToRFBServer(client, (char *)&major, 1) ||
      !WriteToRFBServer(client, (char *)&minor, 1) ||
      !ReadFromRFBServer(client, (char *)&status, 1))
  {
    return FALSE;
  }

  if (status != 0)
  {
    rfbClientLog("Server refused VeNCrypt version %d.%d.\n", (int)major, (int)minor);
    return FALSE;
  }

  if (!ReadVeNCryptSecurityType(client, &authScheme)) return FALSE;
  if (!ReadFromRFBServer(client, (char *)&status, 1) || status != 1)
  {
    rfbClientLog("Server refused VeNCrypt authentication %d (%d).\n", authScheme, (int)status);
    return FALSE;
  }
  client->subAuthScheme = authScheme;

  /* Some VeNCrypt security types are anonymous TLS, others are X509 */
  switch (authScheme)
  {
    case rfbVeNCryptTLSNone:
    case rfbVeNCryptTLSVNC:
    case rfbVeNCryptTLSPlain:
#ifdef LIBVNCSERVER_HAVE_SASL
    case rfbVeNCryptTLSSASL:
#endif /* LIBVNCSERVER_HAVE_SASL */
      anonTLS = TRUE;
      break;
    default:
      anonTLS = FALSE;
      break;
  }

  /* Get X509 Credentials if it's not anonymous */
  if (!anonTLS)
  {

    if (!client->GetCredential)
    {
      rfbClientLog("GetCredential callback is not set.\n");
      return FALSE;
    }
    cred = client->GetCredential(client, rfbCredentialTypeX509);
    if (!cred)
    {
      rfbClientLog("Reading credential failed\n");
      return FALSE;
    }
  }

  /* Start up the TLS session */
  if (!InitializeTLSSession(client, anonTLS, cred)) result = FALSE;

  if (!HandshakeTLS(client)) result = FALSE;

  /* We are done here. The caller should continue with client->subAuthScheme
   * to do actual sub authentication.
   */
  if (cred) FreeX509Credential(cred);
  return result;
}