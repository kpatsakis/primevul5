static int v4l2loopback_set_ctrl(struct v4l2_loopback_device *dev, u32 id,
				 s64 val)
{
	switch (id) {
	case CID_KEEP_FORMAT:
		if (val < 0 || val > 1)
			return -EINVAL;
		dev->keep_format = val;
		try_free_buffers(
			dev); /* will only free buffers if !keep_format */
		break;
	case CID_SUSTAIN_FRAMERATE:
		if (val < 0 || val > 1)
			return -EINVAL;
		spin_lock_bh(&dev->lock);
		dev->sustain_framerate = val;
		check_timers(dev);
		spin_unlock_bh(&dev->lock);
		break;
	case CID_TIMEOUT:
		if (val < 0 || val > MAX_TIMEOUT)
			return -EINVAL;
		spin_lock_bh(&dev->lock);
		dev->timeout_jiffies = msecs_to_jiffies(val);
		check_timers(dev);
		spin_unlock_bh(&dev->lock);
		allocate_timeout_image(dev);
		break;
	case CID_TIMEOUT_IMAGE_IO:
		if (val < 0 || val > 1)
			return -EINVAL;
		dev->timeout_image_io = val;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}