static int des3_set_key(struct ssh_cipher_struct *cipher, void *key, void *IV) {
  if (cipher->key == NULL) {
    if (alloc_key(cipher) < 0) {
      return -1;
    }
    if (gcry_cipher_open(&cipher->key[0], GCRY_CIPHER_3DES,
          GCRY_CIPHER_MODE_CBC, 0)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setkey(cipher->key[0], key, 24)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setiv(cipher->key[0], IV, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
  }

  return 0;
}