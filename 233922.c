void sha256_update(SHA256CTX c, const void *data, unsigned long len){
  SHA256_Update(c,data,len);
}