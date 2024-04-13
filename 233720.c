static int futex_unlock_pi(u32 __user *uaddr, int fshared)
{
	struct futex_hash_bucket *hb;
	struct futex_q *this, *next;
	u32 uval;
	struct plist_head *head;
	union futex_key key = FUTEX_KEY_INIT;
	int ret;

retry:
	if (get_user(uval, uaddr))
		return -EFAULT;
	/*
	 * We release only a lock we actually own:
	 */
	if ((uval & FUTEX_TID_MASK) != task_pid_vnr(current))
		return -EPERM;

	ret = get_futex_key(uaddr, fshared, &key);
	if (unlikely(ret != 0))
		goto out;

	hb = hash_futex(&key);
	spin_lock(&hb->lock);

	/*
	 * To avoid races, try to do the TID -> 0 atomic transition
	 * again. If it succeeds then we can return without waking
	 * anyone else up:
	 */
	if (!(uval & FUTEX_OWNER_DIED))
		uval = cmpxchg_futex_value_locked(uaddr, task_pid_vnr(current), 0);


	if (unlikely(uval == -EFAULT))
		goto pi_faulted;
	/*
	 * Rare case: we managed to release the lock atomically,
	 * no need to wake anyone else up:
	 */
	if (unlikely(uval == task_pid_vnr(current)))
		goto out_unlock;

	/*
	 * Ok, other tasks may need to be woken up - check waiters
	 * and do the wakeup if necessary:
	 */
	head = &hb->chain;

	plist_for_each_entry_safe(this, next, head, list) {
		if (!match_futex (&this->key, &key))
			continue;
		ret = wake_futex_pi(uaddr, uval, this);
		/*
		 * The atomic access to the futex value
		 * generated a pagefault, so retry the
		 * user-access and the wakeup:
		 */
		if (ret == -EFAULT)
			goto pi_faulted;
		goto out_unlock;
	}
	/*
	 * No waiters - kernel unlocks the futex:
	 */
	if (!(uval & FUTEX_OWNER_DIED)) {
		ret = unlock_futex_pi(uaddr, uval);
		if (ret == -EFAULT)
			goto pi_faulted;
	}

out_unlock:
	spin_unlock(&hb->lock);
	put_futex_key(fshared, &key);

out:
	return ret;

pi_faulted:
	spin_unlock(&hb->lock);
	put_futex_key(fshared, &key);

	ret = fault_in_user_writeable(uaddr);
	if (!ret)
		goto retry;

	return ret;
}