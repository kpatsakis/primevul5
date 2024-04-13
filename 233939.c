static int des1_set_key(struct ssh_cipher_struct *cipher, void *key, void *IV){
  if(!cipher->key){
    if (alloc_key(cipher) < 0) {
      return -1;
    }
    DES_set_odd_parity(key);
    DES_set_key_unchecked(key,cipher->key);
  }
  cipher->IV=IV;
  return 0;
}