static void tipc_aead_users_inc(struct tipc_aead __rcu *aead, int lim)
{
	struct tipc_aead *tmp;

	rcu_read_lock();
	tmp = rcu_dereference(aead);
	if (tmp)
		atomic_add_unless(&tmp->users, 1, lim);
	rcu_read_unlock();
}