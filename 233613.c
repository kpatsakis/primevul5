int ecryptfs_init_crypt_ctx(struct ecryptfs_crypt_stat *crypt_stat)
{
	char *full_alg_name;
	int rc = -EINVAL;

	if (!crypt_stat->cipher) {
		ecryptfs_printk(KERN_ERR, "No cipher specified\n");
		goto out;
	}
	ecryptfs_printk(KERN_DEBUG,
			"Initializing cipher [%s]; strlen = [%d]; "
			"key_size_bits = [%d]\n",
			crypt_stat->cipher, (int)strlen(crypt_stat->cipher),
			crypt_stat->key_size << 3);
	if (crypt_stat->tfm) {
		rc = 0;
		goto out;
	}
	mutex_lock(&crypt_stat->cs_tfm_mutex);
	rc = ecryptfs_crypto_api_algify_cipher_name(&full_alg_name,
						    crypt_stat->cipher, "cbc");
	if (rc)
		goto out_unlock;
	crypt_stat->tfm = crypto_alloc_blkcipher(full_alg_name, 0,
						 CRYPTO_ALG_ASYNC);
	kfree(full_alg_name);
	if (IS_ERR(crypt_stat->tfm)) {
		rc = PTR_ERR(crypt_stat->tfm);
		ecryptfs_printk(KERN_ERR, "cryptfs: init_crypt_ctx(): "
				"Error initializing cipher [%s]\n",
				crypt_stat->cipher);
		goto out_unlock;
	}
	crypto_blkcipher_set_flags(crypt_stat->tfm, CRYPTO_TFM_REQ_WEAK_KEY);
	rc = 0;
out_unlock:
	mutex_unlock(&crypt_stat->cs_tfm_mutex);
out:
	return rc;
}