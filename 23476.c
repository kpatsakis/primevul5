static int __kprobes notifier_call_chain(struct notifier_block **nl,
		unsigned long val, void *v)
{
	int ret = NOTIFY_DONE;
	struct notifier_block *nb, *next_nb;

	nb = rcu_dereference(*nl);
	while (nb) {
		next_nb = rcu_dereference(nb->next);
		ret = nb->notifier_call(nb, val, v);
		if ((ret & NOTIFY_STOP_MASK) == NOTIFY_STOP_MASK)
			break;
		nb = next_nb;
	}
	return ret;
}