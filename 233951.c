void md5_update(MD5CTX c, const void *data, unsigned long len) {
    gcry_md_write(c,data,len);
}