int srcu_notifier_call_chain(struct srcu_notifier_head *nh,
		unsigned long val, void *v)
{
	int ret;
	int idx;

	idx = srcu_read_lock(&nh->srcu);
	ret = notifier_call_chain(&nh->head, val, v);
	srcu_read_unlock(&nh->srcu, idx);
	return ret;
}