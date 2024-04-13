void hmac_final(HMACCTX c, unsigned char *hashmacbuf, unsigned int *len) {
  *len = gcry_md_get_algo_dlen(gcry_md_get_algo(c));
  memcpy(hashmacbuf, gcry_md_read(c, 0), *len);
  gcry_md_close(c);
}