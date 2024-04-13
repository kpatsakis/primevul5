static __poll_t ca8210_test_int_poll(
	struct file *filp,
	struct poll_table_struct *ptable
)
{
	__poll_t return_flags = 0;
	struct ca8210_priv *priv = filp->private_data;

	poll_wait(filp, &priv->test.readq, ptable);
	if (!kfifo_is_empty(&priv->test.up_fifo))
		return_flags |= (EPOLLIN | EPOLLRDNORM);
	if (wait_event_interruptible(
		priv->test.readq,
		!kfifo_is_empty(&priv->test.up_fifo))) {
		return EPOLLERR;
	}
	return return_flags;
}