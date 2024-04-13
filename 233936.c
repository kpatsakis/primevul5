void ssh_mac_final(unsigned char *md, ssh_mac_ctx ctx) {
  size_t len;
  switch(ctx->mac_type){
    case SSH_MAC_SHA1:
      len=SHA_DIGEST_LEN;
      break;
    case SSH_MAC_SHA256:
      len=SHA256_DIGEST_LENGTH;
      break;
    case SSH_MAC_SHA384:
      len=SHA384_DIGEST_LENGTH;
      break;
    case SSH_MAC_SHA512:
      len=SHA512_DIGEST_LENGTH;
      break;
  }
  gcry_md_final(ctx->ctx);
  memcpy(md, gcry_md_read(ctx->ctx, 0), len);
  gcry_md_close(ctx->ctx);
  SAFE_FREE(ctx);
}