void sha1(unsigned char *digest, int len, unsigned char *hash) {
  gcry_md_hash_buffer(GCRY_MD_SHA1, hash, digest, len);
}