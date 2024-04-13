MD5CTX md5_init(void) {
  MD5CTX c = NULL;
  gcry_md_open(&c, GCRY_MD_MD5, 0);

  return c;
}