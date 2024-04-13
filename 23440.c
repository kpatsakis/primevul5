int srcu_notifier_chain_unregister(struct srcu_notifier_head *nh,
		struct notifier_block *n)
{
	int ret;

	/*
	 * This code gets used during boot-up, when task switching is
	 * not yet working and interrupts must remain disabled.  At
	 * such times we must not call mutex_lock().
	 */
	if (unlikely(system_state == SYSTEM_BOOTING))
		return notifier_chain_unregister(&nh->head, n);

	mutex_lock(&nh->mutex);
	ret = notifier_chain_unregister(&nh->head, n);
	mutex_unlock(&nh->mutex);
	synchronize_srcu(&nh->srcu);
	return ret;
}