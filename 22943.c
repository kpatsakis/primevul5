static inline void tipc_aead_put(struct tipc_aead *aead)
{
	if (aead && refcount_dec_and_test(&aead->refcnt))
		call_rcu(&aead->rcu, tipc_aead_free);
}