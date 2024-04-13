void kdb_send_sig(struct task_struct *t, int sig)
{
	static struct task_struct *kdb_prev_t;
	int new_t, ret;
	if (!spin_trylock(&t->sighand->siglock)) {
		kdb_printf("Can't do kill command now.\n"
			   "The sigmask lock is held somewhere else in "
			   "kernel, try again later\n");
		return;
	}
	new_t = kdb_prev_t != t;
	kdb_prev_t = t;
	if (t->state != TASK_RUNNING && new_t) {
		spin_unlock(&t->sighand->siglock);
		kdb_printf("Process is not RUNNING, sending a signal from "
			   "kdb risks deadlock\n"
			   "on the run queue locks. "
			   "The signal has _not_ been sent.\n"
			   "Reissue the kill command if you want to risk "
			   "the deadlock.\n");
		return;
	}
	ret = send_signal(sig, SEND_SIG_PRIV, t, PIDTYPE_PID);
	spin_unlock(&t->sighand->siglock);
	if (ret)
		kdb_printf("Fail to deliver Signal %d to process %d.\n",
			   sig, t->pid);
	else
		kdb_printf("Signal %d is sent to process %d.\n", sig, t->pid);
}