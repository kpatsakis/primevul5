void sha1_final(unsigned char *md, SHACTX c) {
  gcry_md_final(c);
  memcpy(md, gcry_md_read(c, 0), SHA_DIGEST_LEN);
  gcry_md_close(c);
}