open_ssl_connection (rfbClient *client, int sockfd, rfbBool anonTLS, rfbCredential *cred)
{
  SSL_CTX *ssl_ctx = NULL;
  SSL *ssl = NULL;
  int n, finished = 0;
  X509_VERIFY_PARAM *param;
  uint8_t verify_crls;

  if (!(ssl_ctx = SSL_CTX_new(SSLv23_client_method())))
  {
    rfbClientLog("Could not create new SSL context.\n");
    return NULL;
  }

  param = X509_VERIFY_PARAM_new();

  /* Setup verification if not anonymous */
  if (!anonTLS)
  {
    verify_crls = cred->x509Credential.x509CrlVerifyMode;
    if (cred->x509Credential.x509CACertFile)
    {
      if (!SSL_CTX_load_verify_locations(ssl_ctx, cred->x509Credential.x509CACertFile, NULL))
      {
        rfbClientLog("Failed to load CA certificate from %s.\n",
                     cred->x509Credential.x509CACertFile);
        goto error_free_ctx;
      }
    } else {
      rfbClientLog("Using default paths for certificate verification.\n");
      SSL_CTX_set_default_verify_paths (ssl_ctx);
    }

    if (cred->x509Credential.x509CACrlFile)
    {
      if (!load_crls_from_file(cred->x509Credential.x509CACrlFile, ssl_ctx))
      {
        rfbClientLog("CRLs could not be loaded.\n");
        goto error_free_ctx;
      }
      if (verify_crls == rfbX509CrlVerifyNone) verify_crls = rfbX509CrlVerifyAll;
    }

    if (cred->x509Credential.x509ClientCertFile && cred->x509Credential.x509ClientKeyFile)
    {
      if (SSL_CTX_use_certificate_chain_file(ssl_ctx, cred->x509Credential.x509ClientCertFile) != 1)
      {
        rfbClientLog("Client certificate could not be loaded.\n");
        goto error_free_ctx;
      }

      if (SSL_CTX_use_PrivateKey_file(ssl_ctx, cred->x509Credential.x509ClientKeyFile,
                                      SSL_FILETYPE_PEM) != 1)
      {
        rfbClientLog("Client private key could not be loaded.\n");
        goto error_free_ctx;
      }

      if (SSL_CTX_check_private_key(ssl_ctx) == 0) {
        rfbClientLog("Client certificate and private key do not match.\n");
        goto error_free_ctx;
      }
    }

    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);

    if (verify_crls == rfbX509CrlVerifyClient) 
      X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK);
    else if (verify_crls == rfbX509CrlVerifyAll)
      X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);

    if(!X509_VERIFY_PARAM_set1_host(param, client->serverHost, strlen(client->serverHost)))
    {
      rfbClientLog("Could not set server name for verification.\n");
      goto error_free_ctx;
    }
    SSL_CTX_set1_param(ssl_ctx, param);
  }

  if (!(ssl = SSL_new (ssl_ctx)))
  {
    rfbClientLog("Could not create a new SSL session.\n");
    goto error_free_ctx;
  }

  /* TODO: finetune this list, take into account anonTLS bool */
  SSL_set_cipher_list(ssl, "ALL");

  SSL_set_fd (ssl, sockfd);
  SSL_CTX_set_app_data (ssl_ctx, client);

  do
  {
    n = SSL_connect(ssl);
		
    if (n != 1) 
    {
      if (wait_for_data(ssl, n, 1) != 1) 
      {
        finished = 1;
        SSL_shutdown(ssl);

        goto error_free_ssl;
      }
    }
  } while( n != 1 && finished != 1 );

  X509_VERIFY_PARAM_free(param);
  return ssl;

error_free_ssl:
  SSL_free(ssl);

error_free_ctx:
  X509_VERIFY_PARAM_free(param);
  SSL_CTX_free(ssl_ctx);

  return NULL;
}