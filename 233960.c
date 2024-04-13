static int aes_set_encrypt_key(struct ssh_cipher_struct *cipher, void *key,
    void *IV) {
  if (cipher->key == NULL) {
    if (alloc_key(cipher) < 0) {
      return -1;
    }
    if (AES_set_encrypt_key(key,cipher->keysize,cipher->key) < 0) {
      SAFE_FREE(cipher->key);
      return -1;
    }
  }
  cipher->IV=IV;
  return 0;
}