setup_key_sizes(void) {
    memset(&keySizes, 0, sizeof(struct key_sizes));

    keySizes.sym_sig_keyLen = DEFAULT_SYM_SIGNING_KEY_LENGTH;
    keySizes.sym_enc_blockSize = DEFAULT_SYM_ENCRYPTION_BLOCK_SIZE;
    keySizes.sym_enc_keyLen = DEFAULT_SYM_ENCRYPTION_KEY_LENGTH;
    keySizes.sym_sig_size = DEFAULT_SYM_SIGNATURE_SIZE;

    keySizes.asym_lcl_sig_size = DEFAULT_ASYM_LOCAL_SIGNATURE_SIZE;
    keySizes.asym_rmt_sig_size = DEFAULT_ASYM_REMOTE_SIGNATURE_SIZE;

    keySizes.asym_rmt_ptext_blocksize = DEFAULT_ASYM_REMOTE_PLAINTEXT_BLOCKSIZE;
    keySizes.asym_rmt_blocksize = DEFAULT_ASYM_REMOTE_BLOCKSIZE;
    keySizes.asym_rmt_enc_key_size = 2048;
    keySizes.asym_lcl_enc_key_size = 1024;
}