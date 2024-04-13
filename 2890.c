static void free_buffers(struct v4l2_loopback_device *dev)
{
	MARK();
	dprintk("freeing image@%p for dev:%p\n", dev ? dev->image : NULL, dev);
	if (dev->image) {
		vfree(dev->image);
		dev->image = NULL;
	}
	if (dev->timeout_image) {
		vfree(dev->timeout_image);
		dev->timeout_image = NULL;
	}
	dev->imagesize = 0;
}