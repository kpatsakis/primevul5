static ssize_t v4l2_loopback_read(struct file *file, char __user *buf,
				  size_t count, loff_t *ppos)
{
	int read_index;
	struct v4l2_loopback_device *dev;
	struct v4l2_buffer *b;
	MARK();

	dev = v4l2loopback_getdevice(file);

	read_index = get_capture_buffer(file);
	if (read_index < 0)
		return read_index;
	if (count > dev->buffer_size)
		count = dev->buffer_size;
	b = &dev->buffers[read_index].buffer;
	if (count > b->bytesused)
		count = b->bytesused;
	if (copy_to_user((void *)buf, (void *)(dev->image + b->m.offset),
			 count)) {
		printk(KERN_ERR
		       "v4l2-loopback: failed copy_to_user() in read buf\n");
		return -EFAULT;
	}
	dprintkrw("leave v4l2_loopback_read()\n");
	return count;
}