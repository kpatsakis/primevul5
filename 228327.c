static struct urb *iso_alloc_urb(
	struct usb_device	*udev,
	int			pipe,
	struct usb_endpoint_descriptor	*desc,
	long			bytes,
	unsigned offset
)
{
	struct urb		*urb;
	unsigned		i, maxp, packets;

	if (bytes < 0 || !desc)
		return NULL;

	maxp = usb_endpoint_maxp(desc);
	if (udev->speed >= USB_SPEED_SUPER)
		maxp *= ss_isoc_get_packet_num(udev, pipe);
	else
		maxp *= usb_endpoint_maxp_mult(desc);

	packets = DIV_ROUND_UP(bytes, maxp);

	urb = usb_alloc_urb(packets, GFP_KERNEL);
	if (!urb)
		return urb;
	urb->dev = udev;
	urb->pipe = pipe;

	urb->number_of_packets = packets;
	urb->transfer_buffer_length = bytes;
	urb->transfer_buffer = usb_alloc_coherent(udev, bytes + offset,
							GFP_KERNEL,
							&urb->transfer_dma);
	if (!urb->transfer_buffer) {
		usb_free_urb(urb);
		return NULL;
	}
	if (offset) {
		memset(urb->transfer_buffer, GUARD_BYTE, offset);
		urb->transfer_buffer += offset;
		urb->transfer_dma += offset;
	}
	/* For inbound transfers use guard byte so that test fails if
		data not correctly copied */
	memset(urb->transfer_buffer,
			usb_pipein(urb->pipe) ? GUARD_BYTE : 0,
			bytes);

	for (i = 0; i < packets; i++) {
		/* here, only the last packet will be short */
		urb->iso_frame_desc[i].length = min((unsigned) bytes, maxp);
		bytes -= urb->iso_frame_desc[i].length;

		urb->iso_frame_desc[i].offset = maxp * i;
	}

	urb->complete = complicated_callback;
	/* urb->context = SET BY CALLER */
	urb->interval = 1 << (desc->bInterval - 1);
	urb->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
	return urb;
}