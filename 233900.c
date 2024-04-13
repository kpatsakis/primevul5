ssh_mac_ctx ssh_mac_ctx_init(enum ssh_mac_e type){
  ssh_mac_ctx ctx=malloc(sizeof(struct ssh_mac_ctx_struct));
  ctx->mac_type=type;
  switch(type){
    case SSH_MAC_SHA1:
      gcry_md_open(&ctx->ctx, GCRY_MD_SHA1, 0);
      break;
    case SSH_MAC_SHA256:
      gcry_md_open(&ctx->ctx, GCRY_MD_SHA256, 0);
      break;
    case SSH_MAC_SHA384:
      gcry_md_open(&ctx->ctx, GCRY_MD_SHA384, 0);
      break;
    case SSH_MAC_SHA512:
      gcry_md_open(&ctx->ctx, GCRY_MD_SHA512, 0);
      break;
    default:
      SAFE_FREE(ctx);
      return NULL;
  }
  return ctx;
}