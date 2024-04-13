static void aes_decrypt(struct ssh_cipher_struct *cipher, void *in, void *out,
    unsigned long len) {
  gcry_cipher_decrypt(cipher->key[0], out, len, in, len);
}