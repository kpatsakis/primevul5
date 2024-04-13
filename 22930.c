int tipc_crypto_key_init(struct tipc_crypto *c, struct tipc_aead_key *ukey,
			 u8 mode, bool master_key)
{
	struct tipc_aead *aead = NULL;
	int rc = 0;

	/* Initiate with the new user key */
	rc = tipc_aead_init(&aead, ukey, mode);

	/* Attach it to the crypto */
	if (likely(!rc)) {
		rc = tipc_crypto_key_attach(c, aead, 0, master_key);
		if (rc < 0)
			tipc_aead_free(&aead->rcu);
	}

	return rc;
}