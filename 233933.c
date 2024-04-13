HMACCTX hmac_init(const void *key, int len, enum ssh_hmac_e type) {
  HMACCTX c = NULL;

  switch(type) {
    case SSH_HMAC_SHA1:
      gcry_md_open(&c, GCRY_MD_SHA1, GCRY_MD_FLAG_HMAC);
      break;
    case SSH_HMAC_MD5:
      gcry_md_open(&c, GCRY_MD_MD5, GCRY_MD_FLAG_HMAC);
      break;
    default:
      c = NULL;
  }

  gcry_md_setkey(c, key, len);

  return c;
}