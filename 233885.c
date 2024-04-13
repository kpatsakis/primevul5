static void aes_decrypt(struct ssh_cipher_struct *cipher, void *in, void *out,
    unsigned long len) {
  AES_cbc_encrypt(in, out, len, cipher->key, cipher->IV, AES_DECRYPT);
}