int __kprobes atomic_notifier_call_chain(struct atomic_notifier_head *nh,
		unsigned long val, void *v)
{
	int ret;

	rcu_read_lock();
	ret = notifier_call_chain(&nh->head, val, v);
	rcu_read_unlock();
	return ret;
}