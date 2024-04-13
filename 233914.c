static void des3_1_decrypt(struct ssh_cipher_struct *cipher, void *in,
    void *out, unsigned long len) {
  gcry_cipher_decrypt(cipher->key[2], out, len, in, len);
  gcry_cipher_encrypt(cipher->key[1], in, len, out, len);
  gcry_cipher_decrypt(cipher->key[0], out, len, in, len);
}