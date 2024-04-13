static int unlink_queued(struct usbtest_dev *dev, int pipe, unsigned num,
		unsigned size)
{
	struct queued_ctx	ctx;
	struct usb_device	*udev = testdev_to_usbdev(dev);
	void			*buf;
	dma_addr_t		buf_dma;
	int			i;
	int			retval = -ENOMEM;

	init_completion(&ctx.complete);
	atomic_set(&ctx.pending, 1);	/* One more than the actual value */
	ctx.num = num;
	ctx.status = 0;

	buf = usb_alloc_coherent(udev, size, GFP_KERNEL, &buf_dma);
	if (!buf)
		return retval;
	memset(buf, 0, size);

	/* Allocate and init the urbs we'll queue */
	ctx.urbs = kcalloc(num, sizeof(struct urb *), GFP_KERNEL);
	if (!ctx.urbs)
		goto free_buf;
	for (i = 0; i < num; i++) {
		ctx.urbs[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!ctx.urbs[i])
			goto free_urbs;
		usb_fill_bulk_urb(ctx.urbs[i], udev, pipe, buf, size,
				unlink_queued_callback, &ctx);
		ctx.urbs[i]->transfer_dma = buf_dma;
		ctx.urbs[i]->transfer_flags = URB_NO_TRANSFER_DMA_MAP;

		if (usb_pipeout(ctx.urbs[i]->pipe)) {
			simple_fill_buf(ctx.urbs[i]);
			ctx.urbs[i]->transfer_flags |= URB_ZERO_PACKET;
		}
	}

	/* Submit all the URBs and then unlink URBs num - 4 and num - 2. */
	for (i = 0; i < num; i++) {
		atomic_inc(&ctx.pending);
		retval = usb_submit_urb(ctx.urbs[i], GFP_KERNEL);
		if (retval != 0) {
			dev_err(&dev->intf->dev, "submit urbs[%d] fail %d\n",
					i, retval);
			atomic_dec(&ctx.pending);
			ctx.status = retval;
			break;
		}
	}
	if (i == num) {
		usb_unlink_urb(ctx.urbs[num - 4]);
		usb_unlink_urb(ctx.urbs[num - 2]);
	} else {
		while (--i >= 0)
			usb_unlink_urb(ctx.urbs[i]);
	}

	if (atomic_dec_and_test(&ctx.pending))		/* The extra count */
		complete(&ctx.complete);
	wait_for_completion(&ctx.complete);
	retval = ctx.status;

 free_urbs:
	for (i = 0; i < num; i++)
		usb_free_urb(ctx.urbs[i]);
	kfree(ctx.urbs);
 free_buf:
	usb_free_coherent(udev, size, buf, buf_dma);
	return retval;
}