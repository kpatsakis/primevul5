void sha1_update(SHACTX c, const void *data, unsigned long len) {
  SHA1_Update(c,data,len);
}