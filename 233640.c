static int decrypt_scatterlist(struct ecryptfs_crypt_stat *crypt_stat,
			       struct scatterlist *dest_sg,
			       struct scatterlist *src_sg, int size,
			       unsigned char *iv)
{
	struct blkcipher_desc desc = {
		.tfm = crypt_stat->tfm,
		.info = iv,
		.flags = CRYPTO_TFM_REQ_MAY_SLEEP
	};
	int rc = 0;

	/* Consider doing this once, when the file is opened */
	mutex_lock(&crypt_stat->cs_tfm_mutex);
	rc = crypto_blkcipher_setkey(crypt_stat->tfm, crypt_stat->key,
				     crypt_stat->key_size);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error setting key; rc = [%d]\n",
				rc);
		mutex_unlock(&crypt_stat->cs_tfm_mutex);
		rc = -EINVAL;
		goto out;
	}
	ecryptfs_printk(KERN_DEBUG, "Decrypting [%d] bytes.\n", size);
	rc = crypto_blkcipher_decrypt_iv(&desc, dest_sg, src_sg, size);
	mutex_unlock(&crypt_stat->cs_tfm_mutex);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error decrypting; rc = [%d]\n",
				rc);
		goto out;
	}
	rc = size;
out:
	return rc;
}