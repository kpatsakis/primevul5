static int tipc_aead_key_generate(struct tipc_aead_key *skey)
{
	int rc = 0;

	/* Fill the key's content with a random value via RNG cipher */
	rc = crypto_get_default_rng();
	if (likely(!rc)) {
		rc = crypto_rng_get_bytes(crypto_default_rng, skey->key,
					  skey->keylen);
		crypto_put_default_rng();
	}

	return rc;
}