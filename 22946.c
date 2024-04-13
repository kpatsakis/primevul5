static void tipc_aead_users_set(struct tipc_aead __rcu *aead, int val)
{
	struct tipc_aead *tmp;
	int cur;

	rcu_read_lock();
	tmp = rcu_dereference(aead);
	if (tmp) {
		do {
			cur = atomic_read(&tmp->users);
			if (cur == val)
				break;
		} while (atomic_cmpxchg(&tmp->users, cur, val) != cur);
	}
	rcu_read_unlock();
}