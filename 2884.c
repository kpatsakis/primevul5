static void try_free_buffers(struct v4l2_loopback_device *dev)
{
	MARK();
	if (0 == dev->open_count.counter && !dev->keep_format) {
		free_buffers(dev);
		dev->ready_for_capture = 0;
		dev->buffer_size = 0;
		dev->write_position = 0;
	}
}