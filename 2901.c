static int vidioc_reqbufs(struct file *file, void *fh,
			  struct v4l2_requestbuffers *b)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	int i;
	MARK();

	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(fh);

	dprintk("reqbufs: %d\t%d=%d\n", b->memory, b->count,
		dev->buffers_number);
	if (opener->timeout_image_io) {
		if (b->memory != V4L2_MEMORY_MMAP)
			return -EINVAL;
		b->count = 1;
		return 0;
	}

	init_buffers(dev);
	switch (b->memory) {
	case V4L2_MEMORY_MMAP:
		/* do nothing here, buffers are always allocated */
		if (b->count < 1 || dev->buffers_number < 1)
			return 0;

		if (b->count > dev->buffers_number)
			b->count = dev->buffers_number;

		/* make sure that outbufs_list contains buffers from 0 to used_buffers-1
		 * actually, it will have been already populated via v4l2_loopback_init()
		 * at this point */
		if (list_empty(&dev->outbufs_list)) {
			for (i = 0; i < dev->used_buffers; ++i)
				list_add_tail(&dev->buffers[i].list_head,
					      &dev->outbufs_list);
		}

		/* also, if dev->used_buffers is going to be decreased, we should remove
		 * out-of-range buffers from outbufs_list, and fix bufpos2index mapping */
		if (b->count < dev->used_buffers) {
			struct v4l2l_buffer *pos, *n;

			list_for_each_entry_safe (pos, n, &dev->outbufs_list,
						  list_head) {
				if (pos->buffer.index >= b->count)
					list_del(&pos->list_head);
			}

			/* after we update dev->used_buffers, buffers in outbufs_list will
			 * correspond to dev->write_position + [0;b->count-1] range */
			i = dev->write_position;
			list_for_each_entry (pos, &dev->outbufs_list,
					     list_head) {
				dev->bufpos2index[i % b->count] =
					pos->buffer.index;
				++i;
			}
		}

		opener->buffers_number = b->count;
		if (opener->buffers_number < dev->used_buffers)
			dev->used_buffers = opener->buffers_number;
		return 0;
	default:
		return -EINVAL;
	}
}