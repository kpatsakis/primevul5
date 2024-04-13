static int vidioc_qbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	struct v4l2l_buffer *b;
	int index;

	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(fh);

	if (buf->index > max_buffers)
		return -EINVAL;
	if (opener->timeout_image_io)
		return 0;

	index = buf->index % dev->used_buffers;
	b = &dev->buffers[index];

	switch (buf->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		dprintkrw("capture QBUF index: %d\n", index);
		set_queued(b);
		return 0;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		dprintkrw("output QBUF pos: %d index: %d\n",
			  dev->write_position, index);
		if (buf->timestamp.tv_sec == 0 && buf->timestamp.tv_usec == 0)
			v4l2l_get_timestamp(&b->buffer);
		else
			b->buffer.timestamp = buf->timestamp;
		b->buffer.bytesused = buf->bytesused;
		set_done(b);
		buffer_written(dev, b);

		/*  Hopefully fix 'DQBUF return bad index if queue bigger then 2 for capture'
                    https://github.com/umlaeute/v4l2loopback/issues/60 */
		buf->flags &= ~V4L2_BUF_FLAG_DONE;
		buf->flags |= V4L2_BUF_FLAG_QUEUED;

		wake_up_all(&dev->read_event);
		return 0;
	default:
		return -EINVAL;
	}
}