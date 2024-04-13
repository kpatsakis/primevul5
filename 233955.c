void ssh_mac_update(ssh_mac_ctx ctx, const void *data, unsigned long len) {
  switch(ctx->mac_type){
    case SSH_MAC_SHA1:
      sha1_update(ctx->ctx.sha1_ctx, data, len);
      break;
    case SSH_MAC_SHA256:
      sha256_update(ctx->ctx.sha256_ctx, data, len);
      break;
    case SSH_MAC_SHA384:
    case SSH_MAC_SHA512:
    default:
      break;
  }
}