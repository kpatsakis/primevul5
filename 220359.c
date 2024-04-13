void tty_open_proc_set_tty(struct file *filp, struct tty_struct *tty)
{
	read_lock(&tasklist_lock);
	spin_lock_irq(&current->sighand->siglock);
	if (current->signal->leader &&
	    !current->signal->tty &&
	    tty->session == NULL) {
		/*
		 * Don't let a process that only has write access to the tty
		 * obtain the privileges associated with having a tty as
		 * controlling terminal (being able to reopen it with full
		 * access through /dev/tty, being able to perform pushback).
		 * Many distributions set the group of all ttys to "tty" and
		 * grant write-only access to all terminals for setgid tty
		 * binaries, which should not imply full privileges on all ttys.
		 *
		 * This could theoretically break old code that performs open()
		 * on a write-only file descriptor. In that case, it might be
		 * necessary to also permit this if
		 * inode_permission(inode, MAY_READ) == 0.
		 */
		if (filp->f_mode & FMODE_READ)
			__proc_set_tty(tty);
	}
	spin_unlock_irq(&current->sighand->siglock);
	read_unlock(&tasklist_lock);
}