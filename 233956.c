HMACCTX hmac_init(const void *key, int len, enum ssh_hmac_e type) {
  HMACCTX ctx = NULL;

  ctx = malloc(sizeof(*ctx));
  if (ctx == NULL) {
    return NULL;
  }

#ifndef OLD_CRYPTO
  HMAC_CTX_init(ctx); // openssl 0.9.7 requires it.
#endif

  switch(type) {
    case SSH_HMAC_SHA1:
      HMAC_Init(ctx, key, len, EVP_sha1());
      break;
    case SSH_HMAC_MD5:
      HMAC_Init(ctx, key, len, EVP_md5());
      break;
    default:
      SAFE_FREE(ctx);
      ctx = NULL;
  }

  return ctx;
}