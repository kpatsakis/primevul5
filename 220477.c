static struct nft_rule **nf_tables_chain_alloc_rules(const struct nft_chain *chain,
						     unsigned int alloc)
{
	if (alloc > INT_MAX)
		return NULL;

	alloc += 1;	/* NULL, ends rules */
	if (sizeof(struct nft_rule *) > INT_MAX / alloc)
		return NULL;

	alloc *= sizeof(struct nft_rule *);
	alloc += sizeof(struct nft_rules_old);

	return kvmalloc(alloc, GFP_KERNEL);
}