static void nft_basechain_hook_init(struct nf_hook_ops *ops, u8 family,
				    const struct nft_chain_hook *hook,
				    struct nft_chain *chain)
{
	ops->pf		= family;
	ops->hooknum	= hook->num;
	ops->priority	= hook->priority;
	ops->priv	= chain;
	ops->hook	= hook->type->hooks[ops->hooknum];
}