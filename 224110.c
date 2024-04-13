force_sig_info_to_task(struct kernel_siginfo *info, struct task_struct *t)
{
	unsigned long int flags;
	int ret, blocked, ignored;
	struct k_sigaction *action;
	int sig = info->si_signo;

	spin_lock_irqsave(&t->sighand->siglock, flags);
	action = &t->sighand->action[sig-1];
	ignored = action->sa.sa_handler == SIG_IGN;
	blocked = sigismember(&t->blocked, sig);
	if (blocked || ignored) {
		action->sa.sa_handler = SIG_DFL;
		if (blocked) {
			sigdelset(&t->blocked, sig);
			recalc_sigpending_and_wake(t);
		}
	}
	/*
	 * Don't clear SIGNAL_UNKILLABLE for traced tasks, users won't expect
	 * debugging to leave init killable.
	 */
	if (action->sa.sa_handler == SIG_DFL && !t->ptrace)
		t->signal->flags &= ~SIGNAL_UNKILLABLE;
	ret = send_signal(sig, info, t, PIDTYPE_PID);
	spin_unlock_irqrestore(&t->sighand->siglock, flags);

	return ret;
}