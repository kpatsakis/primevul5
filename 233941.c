void sha256_final(unsigned char *md, SHA256CTX c) {
  SHA256_Final(md, c);
  SAFE_FREE(c);
}