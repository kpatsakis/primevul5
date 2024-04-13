void sha1_final(unsigned char *md, SHACTX c) {
  SHA1_Final(md, c);
  SAFE_FREE(c);
}