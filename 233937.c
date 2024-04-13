static void des1_1_decrypt(struct ssh_cipher_struct *cipher, void *in, void *out,
                           unsigned long len){

  DES_ncbc_encrypt(in,out,len, cipher->key, cipher->IV, 0);
}