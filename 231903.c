static void lp_error (int minor)
{
	DEFINE_WAIT(wait);
	int polling;

	if (LP_F(minor) & LP_ABORT)
		return;

	polling = lp_table[minor].dev->port->irq == PARPORT_IRQ_NONE;
	if (polling) lp_release_parport (&lp_table[minor]);
	prepare_to_wait(&lp_table[minor].waitq, &wait, TASK_INTERRUPTIBLE);
	schedule_timeout(LP_TIMEOUT_POLLED);
	finish_wait(&lp_table[minor].waitq, &wait);
	if (polling) lp_claim_parport_or_block (&lp_table[minor]);
	else parport_yield_blocking (lp_table[minor].dev);
}