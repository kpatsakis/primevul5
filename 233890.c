static void des3_1_encrypt(struct ssh_cipher_struct *cipher, void *in,
    void *out, unsigned long len) {
#ifdef DEBUG_CRYPTO
  ssh_print_hexa("Encrypt IV before", cipher->IV, 24);
#endif
  DES_ncbc_encrypt(in, out, len, cipher->key, cipher->IV, 1);
  DES_ncbc_encrypt(out, in, len, (void*)((uint8_t*)cipher->key + sizeof(DES_key_schedule)),
      (void*)((uint8_t*)cipher->IV + 8), 0);
  DES_ncbc_encrypt(in, out, len, (void*)((uint8_t*)cipher->key + 2 * sizeof(DES_key_schedule)),
      (void*)((uint8_t*)cipher->IV + 16), 1);
#ifdef DEBUG_CRYPTO
  ssh_print_hexa("Encrypt IV after", cipher->IV, 24);
#endif
}