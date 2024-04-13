static void aes_ctr128_encrypt(struct ssh_cipher_struct *cipher, void *in, void *out,
    unsigned long len) {
  unsigned char tmp_buffer[128/8];
  unsigned int num=0;
  /* Some things are special with ctr128 :
   * In this case, tmp_buffer is not being used, because it is used to store temporary data
   * when an encryption is made on lengths that are not multiple of blocksize.
   * Same for num, which is being used to store the current offset in blocksize in CTR
   * function.
   */
  AES_ctr128_encrypt(in, out, len, cipher->key, cipher->IV, tmp_buffer, &num);
}