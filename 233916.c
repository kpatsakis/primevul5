void md5_final(unsigned char *md, MD5CTX c) {
  gcry_md_final(c);
  memcpy(md, gcry_md_read(c, 0), MD5_DIGEST_LEN);
  gcry_md_close(c);
}