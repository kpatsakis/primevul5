static void des3_decrypt(struct ssh_cipher_struct *cipher, void *in,
    void *out, unsigned long len) {
  DES_ede3_cbc_encrypt(in, out, len, cipher->key,
      (void*)((uint8_t*)cipher->key + sizeof(DES_key_schedule)),
      (void*)((uint8_t*)cipher->key + 2 * sizeof(DES_key_schedule)),
      cipher->IV, 0);
}