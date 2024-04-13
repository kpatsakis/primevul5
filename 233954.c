static void des3_encrypt(struct ssh_cipher_struct *cipher, void *in,
    void *out, unsigned long len) {
  gcry_cipher_encrypt(cipher->key[0], out, len, in, len);
}