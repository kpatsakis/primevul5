static int fixup_owner(u32 __user *uaddr, int fshared, struct futex_q *q,
		       int locked)
{
	struct task_struct *owner;
	int ret = 0;

	if (locked) {
		/*
		 * Got the lock. We might not be the anticipated owner if we
		 * did a lock-steal - fix up the PI-state in that case:
		 */
		if (q->pi_state->owner != current)
			ret = fixup_pi_state_owner(uaddr, q, current, fshared);
		goto out;
	}

	/*
	 * Catch the rare case, where the lock was released when we were on the
	 * way back before we locked the hash bucket.
	 */
	if (q->pi_state->owner == current) {
		/*
		 * Try to get the rt_mutex now. This might fail as some other
		 * task acquired the rt_mutex after we removed ourself from the
		 * rt_mutex waiters list.
		 */
		if (rt_mutex_trylock(&q->pi_state->pi_mutex)) {
			locked = 1;
			goto out;
		}

		/*
		 * pi_state is incorrect, some other task did a lock steal and
		 * we returned due to timeout or signal without taking the
		 * rt_mutex. Too late. We can access the rt_mutex_owner without
		 * locking, as the other task is now blocked on the hash bucket
		 * lock. Fix the state up.
		 */
		owner = rt_mutex_owner(&q->pi_state->pi_mutex);
		ret = fixup_pi_state_owner(uaddr, q, owner, fshared);
		goto out;
	}

	/*
	 * Paranoia check. If we did not take the lock, then we should not be
	 * the owner, nor the pending owner, of the rt_mutex.
	 */
	if (rt_mutex_owner(&q->pi_state->pi_mutex) == current)
		printk(KERN_ERR "fixup_owner: ret = %d pi-mutex: %p "
				"pi-state %p\n", ret,
				q->pi_state->pi_mutex.owner,
				q->pi_state->owner);

out:
	return ret ? ret : locked;
}