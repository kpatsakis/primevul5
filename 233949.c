void sha1_update(SHACTX c, const void *data, unsigned long len) {
  gcry_md_write(c, data, len);
}