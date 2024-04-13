static int tipc_aead_users(struct tipc_aead __rcu *aead)
{
	struct tipc_aead *tmp;
	int users = 0;

	rcu_read_lock();
	tmp = rcu_dereference(aead);
	if (tmp)
		users = atomic_read(&tmp->users);
	rcu_read_unlock();

	return users;
}