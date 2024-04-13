int tipc_aead_key_validate(struct tipc_aead_key *ukey, struct genl_info *info)
{
	int keylen;

	/* Check if algorithm exists */
	if (unlikely(!crypto_has_alg(ukey->alg_name, 0, 0))) {
		GENL_SET_ERR_MSG(info, "unable to load the algorithm (module existed?)");
		return -ENODEV;
	}

	/* Currently, we only support the "gcm(aes)" cipher algorithm */
	if (strcmp(ukey->alg_name, "gcm(aes)")) {
		GENL_SET_ERR_MSG(info, "not supported yet the algorithm");
		return -ENOTSUPP;
	}

	/* Check if key size is correct */
	keylen = ukey->keylen - TIPC_AES_GCM_SALT_SIZE;
	if (unlikely(keylen != TIPC_AES_GCM_KEY_SIZE_128 &&
		     keylen != TIPC_AES_GCM_KEY_SIZE_192 &&
		     keylen != TIPC_AES_GCM_KEY_SIZE_256)) {
		GENL_SET_ERR_MSG(info, "incorrect key length (20, 28 or 36 octets?)");
		return -EKEYREJECTED;
	}

	return 0;
}