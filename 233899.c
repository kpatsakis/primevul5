void md5_final(unsigned char *md, MD5CTX c) {
  MD5_Final(md,c);
  SAFE_FREE(c);
}