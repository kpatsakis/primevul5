static int tipc_aead_clone(struct tipc_aead **dst, struct tipc_aead *src)
{
	struct tipc_aead *aead;
	int cpu;

	if (!src)
		return -ENOKEY;

	if (src->mode != CLUSTER_KEY)
		return -EINVAL;

	if (unlikely(*dst))
		return -EEXIST;

	aead = kzalloc(sizeof(*aead), GFP_ATOMIC);
	if (unlikely(!aead))
		return -ENOMEM;

	aead->tfm_entry = alloc_percpu_gfp(struct tipc_tfm *, GFP_ATOMIC);
	if (unlikely(!aead->tfm_entry)) {
		kfree_sensitive(aead);
		return -ENOMEM;
	}

	for_each_possible_cpu(cpu) {
		*per_cpu_ptr(aead->tfm_entry, cpu) =
				*per_cpu_ptr(src->tfm_entry, cpu);
	}

	memcpy(aead->hint, src->hint, sizeof(src->hint));
	aead->mode = src->mode;
	aead->salt = src->salt;
	aead->authsize = src->authsize;
	atomic_set(&aead->users, 0);
	atomic64_set(&aead->seqno, 0);
	refcount_set(&aead->refcnt, 1);

	WARN_ON(!refcount_inc_not_zero(&src->refcnt));
	aead->cloned = src;

	*dst = aead;
	return 0;
}