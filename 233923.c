static int des3_set_key(struct ssh_cipher_struct *cipher, void *key,void *IV) {
  if (cipher->key == NULL) {
    if (alloc_key(cipher) < 0) {
      return -1;
    }

    DES_set_odd_parity(key);
    DES_set_odd_parity((void*)((uint8_t*)key + 8));
    DES_set_odd_parity((void*)((uint8_t*)key + 16));
    DES_set_key_unchecked(key, cipher->key);
    DES_set_key_unchecked((void*)((uint8_t*)key + 8), (void*)((uint8_t*)cipher->key + sizeof(DES_key_schedule)));
    DES_set_key_unchecked((void*)((uint8_t*)key + 16), (void*)((uint8_t*)cipher->key + 2 * sizeof(DES_key_schedule)));
  }
  cipher->IV=IV;
  return 0;
}