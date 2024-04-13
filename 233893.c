void ssh_mac_update(ssh_mac_ctx ctx, const void *data, unsigned long len) {
  gcry_md_write(ctx->ctx,data,len);
}