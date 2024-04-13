static struct tipc_aead *tipc_aead_get(struct tipc_aead __rcu *aead)
{
	struct tipc_aead *tmp;

	rcu_read_lock();
	tmp = rcu_dereference(aead);
	if (unlikely(!tmp || !refcount_inc_not_zero(&tmp->refcnt)))
		tmp = NULL;
	rcu_read_unlock();

	return tmp;
}