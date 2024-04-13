void hmac_update(HMACCTX ctx, const void *data, unsigned long len) {
  HMAC_Update(ctx, data, len);
}