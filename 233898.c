static int des3_1_set_key(struct ssh_cipher_struct *cipher, void *key, void *IV) {
  if (cipher->key == NULL) {
    if (alloc_key(cipher) < 0) {
      return -1;
    }
    if (gcry_cipher_open(&cipher->key[0], GCRY_CIPHER_DES,
          GCRY_CIPHER_MODE_CBC, 0)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setkey(cipher->key[0], key, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setiv(cipher->key[0], IV, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }

    if (gcry_cipher_open(&cipher->key[1], GCRY_CIPHER_DES,
          GCRY_CIPHER_MODE_CBC, 0)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setkey(cipher->key[1], (unsigned char *)key + 8, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setiv(cipher->key[1], (unsigned char *)IV + 8, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }

    if (gcry_cipher_open(&cipher->key[2], GCRY_CIPHER_DES,
          GCRY_CIPHER_MODE_CBC, 0)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setkey(cipher->key[2], (unsigned char *)key + 16, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
    if (gcry_cipher_setiv(cipher->key[2], (unsigned char *)IV + 16, 8)) {
      SAFE_FREE(cipher->key);
      return -1;
    }
  }

  return 0;
}