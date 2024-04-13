static int v4l2_loopback_mmap(struct file *file, struct vm_area_struct *vma)
{
	u8 *addr;
	unsigned long start;
	unsigned long size;
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	struct v4l2l_buffer *buffer = NULL;
	MARK();

	start = (unsigned long)vma->vm_start;
	size = (unsigned long)(vma->vm_end - vma->vm_start);

	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(file->private_data);

	if (size > dev->buffer_size) {
		dprintk("userspace tries to mmap too much, fail\n");
		return -EINVAL;
	}
	if (opener->timeout_image_io) {
		/* we are going to map the timeout_image_buffer */
		if ((vma->vm_pgoff << PAGE_SHIFT) !=
		    dev->buffer_size * MAX_BUFFERS) {
			dprintk("invalid mmap offset for timeout_image_io mode\n");
			return -EINVAL;
		}
	} else if ((vma->vm_pgoff << PAGE_SHIFT) >
		   dev->buffer_size * (dev->buffers_number - 1)) {
		dprintk("userspace tries to mmap too far, fail\n");
		return -EINVAL;
	}

	/* FIXXXXXME: allocation should not happen here! */
	if (NULL == dev->image)
		if (allocate_buffers(dev) < 0)
			return -EINVAL;

	if (opener->timeout_image_io) {
		buffer = &dev->timeout_image_buffer;
		addr = dev->timeout_image;
	} else {
		int i;
		for (i = 0; i < dev->buffers_number; ++i) {
			buffer = &dev->buffers[i];
			if ((buffer->buffer.m.offset >> PAGE_SHIFT) ==
			    vma->vm_pgoff)
				break;
		}

		if (NULL == buffer)
			return -EINVAL;

		addr = dev->image + (vma->vm_pgoff << PAGE_SHIFT);
	}

	while (size > 0) {
		struct page *page;

		page = vmalloc_to_page(addr);

		if (vm_insert_page(vma, start, page) < 0)
			return -EAGAIN;

		start += PAGE_SIZE;
		addr += PAGE_SIZE;
		size -= PAGE_SIZE;
	}

	vma->vm_ops = &vm_ops;
	vma->vm_private_data = buffer;
	buffer->buffer.flags |= V4L2_BUF_FLAG_MAPPED;

	vm_open(vma);

	MARK();
	return 0;
}