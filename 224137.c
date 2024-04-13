void kernel_sigaction(int sig, __sighandler_t action)
{
	spin_lock_irq(&current->sighand->siglock);
	current->sighand->action[sig - 1].sa.sa_handler = action;
	if (action == SIG_IGN) {
		sigset_t mask;

		sigemptyset(&mask);
		sigaddset(&mask, sig);

		flush_sigqueue_mask(&mask, &current->signal->shared_pending);
		flush_sigqueue_mask(&mask, &current->pending);
		recalc_sigpending();
	}
	spin_unlock_irq(&current->sighand->siglock);
}