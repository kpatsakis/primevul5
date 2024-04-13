static int vidioc_querybuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
	enum v4l2_buf_type type;
	int index;
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;

	MARK();

	type = b->type;
	index = b->index;
	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(fh);

	if ((b->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) &&
	    (b->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)) {
		return -EINVAL;
	}
	if (b->index > max_buffers)
		return -EINVAL;

	if (opener->timeout_image_io)
		*b = dev->timeout_image_buffer.buffer;
	else
		*b = dev->buffers[b->index % dev->used_buffers].buffer;

	b->type = type;
	b->index = index;
	dprintkrw("buffer type: %d (of %d with size=%ld)\n", b->memory,
		  dev->buffers_number, dev->buffer_size);

	/*  Hopefully fix 'DQBUF return bad index if queue bigger then 2 for capture'
            https://github.com/umlaeute/v4l2loopback/issues/60 */
	b->flags &= ~V4L2_BUF_FLAG_DONE;
	b->flags |= V4L2_BUF_FLAG_QUEUED;

	return 0;
}