static void init_buffers(struct v4l2_loopback_device *dev)
{
	int i;
	int buffer_size;
	int bytesused;
	MARK();

	buffer_size = dev->buffer_size;
	bytesused = dev->pix_format.sizeimage;

	for (i = 0; i < dev->buffers_number; ++i) {
		struct v4l2_buffer *b = &dev->buffers[i].buffer;
		b->index = i;
		b->bytesused = bytesused;
		b->length = buffer_size;
		b->field = V4L2_FIELD_NONE;
		b->flags = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 1)
		b->input = 0;
#endif
		b->m.offset = i * buffer_size;
		b->memory = V4L2_MEMORY_MMAP;
		b->sequence = 0;
		b->timestamp.tv_sec = 0;
		b->timestamp.tv_usec = 0;
		b->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		v4l2l_get_timestamp(b);
	}
	dev->timeout_image_buffer = dev->buffers[0];
	dev->timeout_image_buffer.buffer.m.offset = MAX_BUFFERS * buffer_size;
	MARK();
}