void ecryptfs_destroy_crypt_stat(struct ecryptfs_crypt_stat *crypt_stat)
{
	struct ecryptfs_key_sig *key_sig, *key_sig_tmp;

	if (crypt_stat->tfm)
		crypto_free_blkcipher(crypt_stat->tfm);
	if (crypt_stat->hash_tfm)
		crypto_free_hash(crypt_stat->hash_tfm);
	mutex_lock(&crypt_stat->keysig_list_mutex);
	list_for_each_entry_safe(key_sig, key_sig_tmp,
				 &crypt_stat->keysig_list, crypt_stat_list) {
		list_del(&key_sig->crypt_stat_list);
		kmem_cache_free(ecryptfs_key_sig_cache, key_sig);
	}
	mutex_unlock(&crypt_stat->keysig_list_mutex);
	memset(crypt_stat, 0, sizeof(struct ecryptfs_crypt_stat));
}