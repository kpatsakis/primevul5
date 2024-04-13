static int allocate_timeout_image(struct v4l2_loopback_device *dev)
{
	MARK();
	if (dev->buffer_size <= 0)
		return -EINVAL;

	if (dev->timeout_image == NULL) {
		dev->timeout_image = v4l2l_vzalloc(dev->buffer_size);
		if (dev->timeout_image == NULL)
			return -ENOMEM;
	}
	return 0;
}