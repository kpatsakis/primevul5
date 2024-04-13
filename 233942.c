ssh_mac_ctx ssh_mac_ctx_init(enum ssh_mac_e type){
  ssh_mac_ctx ctx=malloc(sizeof(struct ssh_mac_ctx_struct));
  ctx->mac_type=type;
  switch(type){
    case SSH_MAC_SHA1:
      ctx->ctx.sha1_ctx = sha1_init();
      return ctx;
    case SSH_MAC_SHA256:
      ctx->ctx.sha256_ctx = sha256_init();
      return ctx;
    case SSH_MAC_SHA384:
    case SSH_MAC_SHA512:
    default:
      SAFE_FREE(ctx);
      return NULL;
  }
}