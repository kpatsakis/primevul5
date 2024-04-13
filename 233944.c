void hmac_final(HMACCTX ctx, unsigned char *hashmacbuf, unsigned int *len) {
  HMAC_Final(ctx,hashmacbuf,len);

#ifndef OLD_CRYPTO
  HMAC_CTX_cleanup(ctx);
#else
  HMAC_cleanup(ctx);
#endif

  SAFE_FREE(ctx);
}