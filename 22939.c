static struct crypto_aead *tipc_aead_tfm_next(struct tipc_aead *aead)
{
	struct tipc_tfm **tfm_entry;
	struct crypto_aead *tfm;

	tfm_entry = get_cpu_ptr(aead->tfm_entry);
	*tfm_entry = list_next_entry(*tfm_entry, list);
	tfm = (*tfm_entry)->tfm;
	put_cpu_ptr(tfm_entry);

	return tfm;
}