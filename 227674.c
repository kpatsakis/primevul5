static ssize_t ca8210_test_int_user_read(
	struct file  *filp,
	char __user  *buf,
	size_t        len,
	loff_t       *offp
)
{
	int i, cmdlen;
	struct ca8210_priv *priv = filp->private_data;
	unsigned char *fifo_buffer;
	unsigned long bytes_not_copied;

	if (filp->f_flags & O_NONBLOCK) {
		/* Non-blocking mode */
		if (kfifo_is_empty(&priv->test.up_fifo))
			return 0;
	} else {
		/* Blocking mode */
		wait_event_interruptible(
			priv->test.readq,
			!kfifo_is_empty(&priv->test.up_fifo)
		);
	}

	if (kfifo_out(&priv->test.up_fifo, &fifo_buffer, 4) != 4) {
		dev_err(
			&priv->spi->dev,
			"test_interface: Wrong number of elements popped from upstream fifo\n"
		);
		return 0;
	}
	cmdlen = fifo_buffer[1];
	bytes_not_copied = cmdlen + 2;

	bytes_not_copied = copy_to_user(buf, fifo_buffer, bytes_not_copied);
	if (bytes_not_copied > 0) {
		dev_err(
			&priv->spi->dev,
			"%lu bytes could not be copied to user space!\n",
			bytes_not_copied
		);
	}

	dev_dbg(&priv->spi->dev, "test_interface: Cmd len = %d\n", cmdlen);

	dev_dbg(&priv->spi->dev, "test_interface: Read\n");
	for (i = 0; i < cmdlen + 2; i++)
		dev_dbg(&priv->spi->dev, "%#03x\n", fifo_buffer[i]);

	kfree(fifo_buffer);

	return cmdlen + 2;
}