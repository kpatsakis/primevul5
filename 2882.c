static int v4l2_loopback_close(struct file *file)
{
	struct v4l2_loopback_opener *opener;
	struct v4l2_loopback_device *dev;
	int iswriter = 0;
	MARK();

	opener = fh_to_opener(file->private_data);
	dev = v4l2loopback_getdevice(file);

	if (WRITER == opener->type)
		iswriter = 1;

	atomic_dec(&dev->open_count);
	if (dev->open_count.counter == 0) {
		del_timer_sync(&dev->sustain_timer);
		del_timer_sync(&dev->timeout_timer);
	}
	try_free_buffers(dev);

	v4l2_fh_del(&opener->fh);
	v4l2_fh_exit(&opener->fh);

	kfree(opener);
	if (iswriter) {
		dev->ready_for_output = 1;
	}
	MARK();
	return 0;
}