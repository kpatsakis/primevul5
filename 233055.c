FreeX509Credential(rfbCredential *cred)
{
  if (cred->x509Credential.x509CACertFile) free(cred->x509Credential.x509CACertFile);
  if (cred->x509Credential.x509CACrlFile) free(cred->x509Credential.x509CACrlFile);
  if (cred->x509Credential.x509ClientCertFile) free(cred->x509Credential.x509ClientCertFile);
  if (cred->x509Credential.x509ClientKeyFile) free(cred->x509Credential.x509ClientKeyFile);
  free(cred);
}