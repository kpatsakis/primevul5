static int futex_requeue(u32 __user *uaddr1, int fshared, u32 __user *uaddr2,
			 int nr_wake, int nr_requeue, u32 *cmpval,
			 int requeue_pi)
{
	union futex_key key1 = FUTEX_KEY_INIT, key2 = FUTEX_KEY_INIT;
	int drop_count = 0, task_count = 0, ret;
	struct futex_pi_state *pi_state = NULL;
	struct futex_hash_bucket *hb1, *hb2;
	struct plist_head *head1;
	struct futex_q *this, *next;
	u32 curval2;

	if (requeue_pi) {
		/*
		 * requeue_pi requires a pi_state, try to allocate it now
		 * without any locks in case it fails.
		 */
		if (refill_pi_state_cache())
			return -ENOMEM;
		/*
		 * requeue_pi must wake as many tasks as it can, up to nr_wake
		 * + nr_requeue, since it acquires the rt_mutex prior to
		 * returning to userspace, so as to not leave the rt_mutex with
		 * waiters and no owner.  However, second and third wake-ups
		 * cannot be predicted as they involve race conditions with the
		 * first wake and a fault while looking up the pi_state.  Both
		 * pthread_cond_signal() and pthread_cond_broadcast() should
		 * use nr_wake=1.
		 */
		if (nr_wake != 1)
			return -EINVAL;
	}

retry:
	if (pi_state != NULL) {
		/*
		 * We will have to lookup the pi_state again, so free this one
		 * to keep the accounting correct.
		 */
		free_pi_state(pi_state);
		pi_state = NULL;
	}

	ret = get_futex_key(uaddr1, fshared, &key1);
	if (unlikely(ret != 0))
		goto out;
	ret = get_futex_key(uaddr2, fshared, &key2);
	if (unlikely(ret != 0))
		goto out_put_key1;

	hb1 = hash_futex(&key1);
	hb2 = hash_futex(&key2);

retry_private:
	double_lock_hb(hb1, hb2);

	if (likely(cmpval != NULL)) {
		u32 curval;

		ret = get_futex_value_locked(&curval, uaddr1);

		if (unlikely(ret)) {
			double_unlock_hb(hb1, hb2);

			ret = get_user(curval, uaddr1);
			if (ret)
				goto out_put_keys;

			if (!fshared)
				goto retry_private;

			put_futex_key(fshared, &key2);
			put_futex_key(fshared, &key1);
			goto retry;
		}
		if (curval != *cmpval) {
			ret = -EAGAIN;
			goto out_unlock;
		}
	}

	if (requeue_pi && (task_count - nr_wake < nr_requeue)) {
		/*
		 * Attempt to acquire uaddr2 and wake the top waiter. If we
		 * intend to requeue waiters, force setting the FUTEX_WAITERS
		 * bit.  We force this here where we are able to easily handle
		 * faults rather in the requeue loop below.
		 */
		ret = futex_proxy_trylock_atomic(uaddr2, hb1, hb2, &key1,
						 &key2, &pi_state, nr_requeue);

		/*
		 * At this point the top_waiter has either taken uaddr2 or is
		 * waiting on it.  If the former, then the pi_state will not
		 * exist yet, look it up one more time to ensure we have a
		 * reference to it.
		 */
		if (ret == 1) {
			WARN_ON(pi_state);
			drop_count++;
			task_count++;
			ret = get_futex_value_locked(&curval2, uaddr2);
			if (!ret)
				ret = lookup_pi_state(curval2, hb2, &key2,
						      &pi_state);
		}

		switch (ret) {
		case 0:
			break;
		case -EFAULT:
			double_unlock_hb(hb1, hb2);
			put_futex_key(fshared, &key2);
			put_futex_key(fshared, &key1);
			ret = fault_in_user_writeable(uaddr2);
			if (!ret)
				goto retry;
			goto out;
		case -EAGAIN:
			/* The owner was exiting, try again. */
			double_unlock_hb(hb1, hb2);
			put_futex_key(fshared, &key2);
			put_futex_key(fshared, &key1);
			cond_resched();
			goto retry;
		default:
			goto out_unlock;
		}
	}

	head1 = &hb1->chain;
	plist_for_each_entry_safe(this, next, head1, list) {
		if (task_count - nr_wake >= nr_requeue)
			break;

		if (!match_futex(&this->key, &key1))
			continue;

		/*
		 * FUTEX_WAIT_REQEUE_PI and FUTEX_CMP_REQUEUE_PI should always
		 * be paired with each other and no other futex ops.
		 */
		if ((requeue_pi && !this->rt_waiter) ||
		    (!requeue_pi && this->rt_waiter)) {
			ret = -EINVAL;
			break;
		}

		/*
		 * Wake nr_wake waiters.  For requeue_pi, if we acquired the
		 * lock, we already woke the top_waiter.  If not, it will be
		 * woken by futex_unlock_pi().
		 */
		if (++task_count <= nr_wake && !requeue_pi) {
			wake_futex(this);
			continue;
		}

		/* Ensure we requeue to the expected futex for requeue_pi. */
		if (requeue_pi && !match_futex(this->requeue_pi_key, &key2)) {
			ret = -EINVAL;
			break;
		}

		/*
		 * Requeue nr_requeue waiters and possibly one more in the case
		 * of requeue_pi if we couldn't acquire the lock atomically.
		 */
		if (requeue_pi) {
			/* Prepare the waiter to take the rt_mutex. */
			atomic_inc(&pi_state->refcount);
			this->pi_state = pi_state;
			ret = rt_mutex_start_proxy_lock(&pi_state->pi_mutex,
							this->rt_waiter,
							this->task, 1);
			if (ret == 1) {
				/* We got the lock. */
				requeue_pi_wake_futex(this, &key2, hb2);
				drop_count++;
				continue;
			} else if (ret) {
				/* -EDEADLK */
				this->pi_state = NULL;
				free_pi_state(pi_state);
				goto out_unlock;
			}
		}
		requeue_futex(this, hb1, hb2, &key2);
		drop_count++;
	}

out_unlock:
	double_unlock_hb(hb1, hb2);

	/*
	 * drop_futex_key_refs() must be called outside the spinlocks. During
	 * the requeue we moved futex_q's from the hash bucket at key1 to the
	 * one at key2 and updated their key pointer.  We no longer need to
	 * hold the references to key1.
	 */
	while (--drop_count >= 0)
		drop_futex_key_refs(&key1);

out_put_keys:
	put_futex_key(fshared, &key2);
out_put_key1:
	put_futex_key(fshared, &key1);
out:
	if (pi_state != NULL)
		free_pi_state(pi_state);
	return ret ? ret : task_count;
}