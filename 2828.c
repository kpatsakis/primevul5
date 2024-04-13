static int get_capture_buffer(struct file *file)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);
	struct v4l2_loopback_opener *opener = fh_to_opener(file->private_data);
	int pos, ret;
	int timeout_happened;

	if ((file->f_flags & O_NONBLOCK) &&
	    (dev->write_position <= opener->read_position &&
	     dev->reread_count <= opener->reread_count &&
	     !dev->timeout_happened))
		return -EAGAIN;
	wait_event_interruptible(dev->read_event, can_read(dev, opener));

	spin_lock_bh(&dev->lock);
	if (dev->write_position == opener->read_position) {
		if (dev->reread_count > opener->reread_count + 2)
			opener->reread_count = dev->reread_count - 1;
		++opener->reread_count;
		pos = (opener->read_position + dev->used_buffers - 1) %
		      dev->used_buffers;
	} else {
		opener->reread_count = 0;
		if (dev->write_position >
		    opener->read_position + dev->used_buffers)
			opener->read_position = dev->write_position - 1;
		pos = opener->read_position % dev->used_buffers;
		++opener->read_position;
	}
	timeout_happened = dev->timeout_happened;
	dev->timeout_happened = 0;
	spin_unlock_bh(&dev->lock);

	ret = dev->bufpos2index[pos];
	if (timeout_happened) {
		if (ret < 0) {
			dprintk("trying to return not mapped buf[%d]\n", ret);
			return -EFAULT;
		}
		/* although allocated on-demand, timeout_image is freed only
		 * in free_buffers(), so we don't need to worry about it being
		 * deallocated suddenly */
		memcpy(dev->image + dev->buffers[ret].buffer.m.offset,
		       dev->timeout_image, dev->buffer_size);
	}
	return ret;
}