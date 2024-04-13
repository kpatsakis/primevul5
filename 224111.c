static int dequeue_synchronous_signal(kernel_siginfo_t *info)
{
	struct task_struct *tsk = current;
	struct sigpending *pending = &tsk->pending;
	struct sigqueue *q, *sync = NULL;

	/*
	 * Might a synchronous signal be in the queue?
	 */
	if (!((pending->signal.sig[0] & ~tsk->blocked.sig[0]) & SYNCHRONOUS_MASK))
		return 0;

	/*
	 * Return the first synchronous signal in the queue.
	 */
	list_for_each_entry(q, &pending->list, list) {
		/* Synchronous signals have a postive si_code */
		if ((q->info.si_code > SI_USER) &&
		    (sigmask(q->info.si_signo) & SYNCHRONOUS_MASK)) {
			sync = q;
			goto next;
		}
	}
	return 0;
next:
	/*
	 * Check if there is another siginfo for the same signal.
	 */
	list_for_each_entry_continue(q, &pending->list, list) {
		if (q->info.si_signo == sync->info.si_signo)
			goto still_pending;
	}

	sigdelset(&pending->signal, sync->info.si_signo);
	recalc_sigpending();
still_pending:
	list_del_init(&sync->list);
	copy_siginfo(info, &sync->info);
	__sigqueue_free(sync);
	return info->si_signo;
}