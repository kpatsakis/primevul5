SHACTX sha1_init(void) {
  SHACTX ctx = NULL;
  gcry_md_open(&ctx, GCRY_MD_SHA1, 0);

  return ctx;
}