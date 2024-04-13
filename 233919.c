void hmac_update(HMACCTX c, const void *data, unsigned long len) {
  gcry_md_write(c, data, len);
}