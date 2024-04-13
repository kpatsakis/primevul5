static int blowfish_set_key(struct ssh_cipher_struct *cipher, void *key, void *IV){
  if (cipher->key == NULL) {
    if (alloc_key(cipher) < 0) {
      return -1;
    }
    BF_set_key(cipher->key, 16, key);
  }
  cipher->IV = IV;
  return 0;
}