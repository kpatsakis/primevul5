static unsigned int v4l2_loopback_poll(struct file *file,
				       struct poll_table_struct *pts)
{
	struct v4l2_loopback_opener *opener;
	struct v4l2_loopback_device *dev;
	__poll_t req_events = poll_requested_events(pts);
	int ret_mask = 0;
	MARK();

	opener = fh_to_opener(file->private_data);
	dev = v4l2loopback_getdevice(file);

	if (req_events & POLLPRI) {
		if (!v4l2_event_pending(&opener->fh))
			poll_wait(file, &opener->fh.wait, pts);
		if (v4l2_event_pending(&opener->fh)) {
			ret_mask |= POLLPRI;
			if (!(req_events & DEFAULT_POLLMASK))
				return ret_mask;
		}
	}

	switch (opener->type) {
	case WRITER:
		ret_mask |= POLLOUT | POLLWRNORM;
		break;
	case READER:
		if (!can_read(dev, opener)) {
			if (ret_mask)
				return ret_mask;
			poll_wait(file, &dev->read_event, pts);
		}
		if (can_read(dev, opener))
			ret_mask |= POLLIN | POLLRDNORM;
		if (v4l2_event_pending(&opener->fh))
			ret_mask |= POLLPRI;
		break;
	default:
		break;
	}

	MARK();
	return ret_mask;
}