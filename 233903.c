static void blowfish_encrypt(struct ssh_cipher_struct *cipher, void *in,
    void *out, unsigned long len) {
  BF_cbc_encrypt(in, out, len, cipher->key, cipher->IV, BF_ENCRYPT);
}