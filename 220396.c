void start_tty(struct tty_struct *tty)
{
	unsigned long flags;

	spin_lock_irqsave(&tty->flow_lock, flags);
	__start_tty(tty);
	spin_unlock_irqrestore(&tty->flow_lock, flags);
}