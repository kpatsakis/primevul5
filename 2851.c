static int allocate_buffers(struct v4l2_loopback_device *dev)
{
	int err;

	MARK();
	/* vfree on close file operation in case no open handles left */

	if (dev->buffer_size < 1 || dev->buffers_number < 1)
		return -EINVAL;

	if ((__LONG_MAX__ / dev->buffer_size) < dev->buffers_number)
		return -ENOSPC;

	if (dev->image) {
		dprintk("allocating buffers again: %ld %ld\n",
			dev->buffer_size * dev->buffers_number, dev->imagesize);
		/* FIXME: prevent double allocation more intelligently! */
		if (dev->buffer_size * dev->buffers_number == dev->imagesize)
			return 0;

		/* if there is only one writer, no problem should occur */
		if (dev->open_count.counter == 1)
			free_buffers(dev);
		else
			return -EINVAL;
	}

	dev->imagesize = (unsigned long)dev->buffer_size *
			 (unsigned long)dev->buffers_number;

	dprintk("allocating %ld = %ldx%d\n", dev->imagesize, dev->buffer_size,
		dev->buffers_number);
	err = -ENOMEM;

	if (dev->timeout_jiffies > 0) {
		err = allocate_timeout_image(dev);
		if (err < 0)
			goto error;
	}

	dev->image = vmalloc(dev->imagesize);
	if (dev->image == NULL)
		goto error;

	dprintk("vmallocated %ld bytes\n", dev->imagesize);
	MARK();

	init_buffers(dev);
	return 0;

error:
	free_buffers(dev);
	return err;
}