printer_fsync(struct file *fd, loff_t start, loff_t end, int datasync)
{
	struct printer_dev	*dev = fd->private_data;
	struct inode *inode = file_inode(fd);
	unsigned long		flags;
	int			tx_list_empty;

	inode_lock(inode);
	spin_lock_irqsave(&dev->lock, flags);

	if (dev->interface < 0) {
		spin_unlock_irqrestore(&dev->lock, flags);
		inode_unlock(inode);
		return -ENODEV;
	}

	tx_list_empty = (likely(list_empty(&dev->tx_reqs)));
	spin_unlock_irqrestore(&dev->lock, flags);

	if (!tx_list_empty) {
		/* Sleep until all data has been sent */
		wait_event_interruptible(dev->tx_flush_wait,
				(likely(list_empty(&dev->tx_reqs_active))));
	}
	inode_unlock(inode);

	return 0;
}