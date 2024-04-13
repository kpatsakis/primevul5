void ssh_mac_final(unsigned char *md, ssh_mac_ctx ctx) {
  switch(ctx->mac_type){
    case SSH_MAC_SHA1:
      sha1_final(md,ctx->ctx.sha1_ctx);
      break;
    case SSH_MAC_SHA256:
      sha256_final(md,ctx->ctx.sha256_ctx);
      break;
    case SSH_MAC_SHA384:
    case SSH_MAC_SHA512:
    default:
      break;
  }
  SAFE_FREE(ctx);
}