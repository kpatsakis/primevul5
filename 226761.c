static int sctp_wait_for_sndbuf(struct sctp_association *asoc, long *timeo_p,
				size_t msg_len)
{
	struct sock *sk = asoc->base.sk;
	long current_timeo = *timeo_p;
	DEFINE_WAIT(wait);
	int err = 0;

	pr_debug("%s: asoc:%p, timeo:%ld, msg_len:%zu\n", __func__, asoc,
		 *timeo_p, msg_len);

	/* Increment the association's refcnt.  */
	sctp_association_hold(asoc);

	/* Wait on the association specific sndbuf space. */
	for (;;) {
		prepare_to_wait_exclusive(&asoc->wait, &wait,
					  TASK_INTERRUPTIBLE);
		if (asoc->base.dead)
			goto do_dead;
		if (!*timeo_p)
			goto do_nonblock;
		if (sk->sk_err || asoc->state >= SCTP_STATE_SHUTDOWN_PENDING)
			goto do_error;
		if (signal_pending(current))
			goto do_interrupted;
		if (sk_under_memory_pressure(sk))
			sk_mem_reclaim(sk);
		if ((int)msg_len <= sctp_wspace(asoc) &&
		    sk_wmem_schedule(sk, msg_len))
			break;

		/* Let another process have a go.  Since we are going
		 * to sleep anyway.
		 */
		release_sock(sk);
		current_timeo = schedule_timeout(current_timeo);
		lock_sock(sk);
		if (sk != asoc->base.sk)
			goto do_error;

		*timeo_p = current_timeo;
	}

out:
	finish_wait(&asoc->wait, &wait);

	/* Release the association's refcnt.  */
	sctp_association_put(asoc);

	return err;

do_dead:
	err = -ESRCH;
	goto out;

do_error:
	err = -EPIPE;
	goto out;

do_interrupted:
	err = sock_intr_errno(*timeo_p);
	goto out;

do_nonblock:
	err = -EAGAIN;
	goto out;
}