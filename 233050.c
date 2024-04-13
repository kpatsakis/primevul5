load_crls_from_file(char *file, SSL_CTX *ssl_ctx)
{
  X509_STORE *st;
  X509_CRL *crl;
  int i;
  int count = 0;
  BIO *bio;
  STACK_OF(X509_INFO) *xis = NULL;
  X509_INFO *xi;

  st = SSL_CTX_get_cert_store(ssl_ctx);

    int rv = 0;

  bio = BIO_new_file(file, "r");
  if (bio == NULL)
    return FALSE;

  xis = PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL);
  BIO_free(bio);

  for (i = 0; i < sk_X509_INFO_num(xis); i++)
  {
    xi = sk_X509_INFO_value(xis, i);
    if (xi->crl)
    {
      X509_STORE_add_crl(st, xi->crl);
      xi->crl = NULL;
      count++;
    }
  }

  sk_X509_INFO_pop_free(xis, X509_INFO_free);

  if (count > 0)
    return TRUE;
  else
    return FALSE;
}