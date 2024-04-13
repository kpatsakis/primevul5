printer_poll(struct file *fd, poll_table *wait)
{
	struct printer_dev	*dev = fd->private_data;
	unsigned long		flags;
	__poll_t		status = 0;

	mutex_lock(&dev->lock_printer_io);
	spin_lock_irqsave(&dev->lock, flags);

	if (dev->interface < 0) {
		spin_unlock_irqrestore(&dev->lock, flags);
		mutex_unlock(&dev->lock_printer_io);
		return EPOLLERR | EPOLLHUP;
	}

	setup_rx_reqs(dev);
	spin_unlock_irqrestore(&dev->lock, flags);
	mutex_unlock(&dev->lock_printer_io);

	poll_wait(fd, &dev->rx_wait, wait);
	poll_wait(fd, &dev->tx_wait, wait);

	spin_lock_irqsave(&dev->lock, flags);
	if (likely(!list_empty(&dev->tx_reqs)))
		status |= EPOLLOUT | EPOLLWRNORM;

	if (likely(dev->current_rx_bytes) ||
			likely(!list_empty(&dev->rx_buffers)))
		status |= EPOLLIN | EPOLLRDNORM;

	spin_unlock_irqrestore(&dev->lock, flags);

	return status;
}