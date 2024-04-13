static int vidioc_dqbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	int index;
	struct v4l2l_buffer *b;

	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(fh);
	if (opener->timeout_image_io) {
		*buf = dev->timeout_image_buffer.buffer;
		return 0;
	}

	switch (buf->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		index = get_capture_buffer(file);
		if (index < 0)
			return index;
		dprintkrw("capture DQBUF pos: %d index: %d\n",
			  opener->read_position - 1, index);
		if (!(dev->buffers[index].buffer.flags &
		      V4L2_BUF_FLAG_MAPPED)) {
			dprintk("trying to return not mapped buf[%d]\n", index);
			return -EINVAL;
		}
		unset_flags(&dev->buffers[index]);
		*buf = dev->buffers[index].buffer;
		return 0;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		b = list_entry(dev->outbufs_list.prev, struct v4l2l_buffer,
			       list_head);
		list_move_tail(&b->list_head, &dev->outbufs_list);
		dprintkrw("output DQBUF index: %d\n", b->buffer.index);
		unset_flags(b);
		*buf = b->buffer;
		buf->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		return 0;
	default:
		return -EINVAL;
	}
}