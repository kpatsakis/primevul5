static int can_read(struct v4l2_loopback_device *dev,
		    struct v4l2_loopback_opener *opener)
{
	int ret;

	spin_lock_bh(&dev->lock);
	check_timers(dev);
	ret = dev->write_position > opener->read_position ||
	      dev->reread_count > opener->reread_count || dev->timeout_happened;
	spin_unlock_bh(&dev->lock);
	return ret;
}