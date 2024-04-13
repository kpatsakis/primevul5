static void __tty_hangup(struct tty_struct *tty, int exit_session)
{
	struct file *cons_filp = NULL;
	struct file *filp, *f = NULL;
	struct tty_file_private *priv;
	int    closecount = 0, n;
	int refs;

	if (!tty)
		return;


	spin_lock(&redirect_lock);
	if (redirect && file_tty(redirect) == tty) {
		f = redirect;
		redirect = NULL;
	}
	spin_unlock(&redirect_lock);

	tty_lock(tty);

	if (test_bit(TTY_HUPPED, &tty->flags)) {
		tty_unlock(tty);
		return;
	}

	/*
	 * Some console devices aren't actually hung up for technical and
	 * historical reasons, which can lead to indefinite interruptible
	 * sleep in n_tty_read().  The following explicitly tells
	 * n_tty_read() to abort readers.
	 */
	set_bit(TTY_HUPPING, &tty->flags);

	/* inuse_filps is protected by the single tty lock,
	   this really needs to change if we want to flush the
	   workqueue with the lock held */
	check_tty_count(tty, "tty_hangup");

	spin_lock(&tty->files_lock);
	/* This breaks for file handles being sent over AF_UNIX sockets ? */
	list_for_each_entry(priv, &tty->tty_files, list) {
		filp = priv->file;
		if (filp->f_op->write == redirected_tty_write)
			cons_filp = filp;
		if (filp->f_op->write != tty_write)
			continue;
		closecount++;
		__tty_fasync(-1, filp, 0);	/* can't block */
		filp->f_op = &hung_up_tty_fops;
	}
	spin_unlock(&tty->files_lock);

	refs = tty_signal_session_leader(tty, exit_session);
	/* Account for the p->signal references we killed */
	while (refs--)
		tty_kref_put(tty);

	tty_ldisc_hangup(tty, cons_filp != NULL);

	spin_lock_irq(&tty->ctrl_lock);
	clear_bit(TTY_THROTTLED, &tty->flags);
	clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);
	put_pid(tty->session);
	put_pid(tty->pgrp);
	tty->session = NULL;
	tty->pgrp = NULL;
	tty->ctrl_status = 0;
	spin_unlock_irq(&tty->ctrl_lock);

	/*
	 * If one of the devices matches a console pointer, we
	 * cannot just call hangup() because that will cause
	 * tty->count and state->count to go out of sync.
	 * So we just call close() the right number of times.
	 */
	if (cons_filp) {
		if (tty->ops->close)
			for (n = 0; n < closecount; n++)
				tty->ops->close(tty, cons_filp);
	} else if (tty->ops->hangup)
		tty->ops->hangup(tty);
	/*
	 * We don't want to have driver/ldisc interactions beyond the ones
	 * we did here. The driver layer expects no calls after ->hangup()
	 * from the ldisc side, which is now guaranteed.
	 */
	set_bit(TTY_HUPPED, &tty->flags);
	clear_bit(TTY_HUPPING, &tty->flags);
	tty_unlock(tty);

	if (f)
		fput(f);
}