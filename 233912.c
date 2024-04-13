void sha256(unsigned char *digest, int len, unsigned char *hash){
  gcry_md_hash_buffer(GCRY_MD_SHA256, hash, digest, len);
}