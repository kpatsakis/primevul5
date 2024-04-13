static int uvc_parse_format(struct uvc_device *dev,
	struct uvc_streaming *streaming, struct uvc_format *format,
	u32 **intervals, unsigned char *buffer, int buflen)
{
	struct usb_interface *intf = streaming->intf;
	struct usb_host_interface *alts = intf->cur_altsetting;
	struct uvc_format_desc *fmtdesc;
	struct uvc_frame *frame;
	const unsigned char *start = buffer;
	unsigned int width_multiplier = 1;
	unsigned int interval;
	unsigned int i, n;
	u8 ftype;

	format->type = buffer[2];
	format->index = buffer[3];

	switch (buffer[2]) {
	case UVC_VS_FORMAT_UNCOMPRESSED:
	case UVC_VS_FORMAT_FRAME_BASED:
		n = buffer[2] == UVC_VS_FORMAT_UNCOMPRESSED ? 27 : 28;
		if (buflen < n) {
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d FORMAT error\n",
			       dev->udev->devnum,
			       alts->desc.bInterfaceNumber);
			return -EINVAL;
		}

		/* Find the format descriptor from its GUID. */
		fmtdesc = uvc_format_by_guid(&buffer[5]);

		if (fmtdesc != NULL) {
			strscpy(format->name, fmtdesc->name,
				sizeof(format->name));
			format->fcc = fmtdesc->fcc;
		} else {
			uvc_printk(KERN_INFO, "Unknown video format %pUl\n",
				&buffer[5]);
			snprintf(format->name, sizeof(format->name), "%pUl\n",
				&buffer[5]);
			format->fcc = 0;
		}

		format->bpp = buffer[21];

		/* Some devices report a format that doesn't match what they
		 * really send.
		 */
		if (dev->quirks & UVC_QUIRK_FORCE_Y8) {
			if (format->fcc == V4L2_PIX_FMT_YUYV) {
				strscpy(format->name, "Greyscale 8-bit (Y8  )",
					sizeof(format->name));
				format->fcc = V4L2_PIX_FMT_GREY;
				format->bpp = 8;
				width_multiplier = 2;
			}
		}

		if (buffer[2] == UVC_VS_FORMAT_UNCOMPRESSED) {
			ftype = UVC_VS_FRAME_UNCOMPRESSED;
		} else {
			ftype = UVC_VS_FRAME_FRAME_BASED;
			if (buffer[27])
				format->flags = UVC_FMT_FLAG_COMPRESSED;
		}
		break;

	case UVC_VS_FORMAT_MJPEG:
		if (buflen < 11) {
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d FORMAT error\n",
			       dev->udev->devnum,
			       alts->desc.bInterfaceNumber);
			return -EINVAL;
		}

		strscpy(format->name, "MJPEG", sizeof(format->name));
		format->fcc = V4L2_PIX_FMT_MJPEG;
		format->flags = UVC_FMT_FLAG_COMPRESSED;
		format->bpp = 0;
		ftype = UVC_VS_FRAME_MJPEG;
		break;

	case UVC_VS_FORMAT_DV:
		if (buflen < 9) {
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d FORMAT error\n",
			       dev->udev->devnum,
			       alts->desc.bInterfaceNumber);
			return -EINVAL;
		}

		switch (buffer[8] & 0x7f) {
		case 0:
			strscpy(format->name, "SD-DV", sizeof(format->name));
			break;
		case 1:
			strscpy(format->name, "SDL-DV", sizeof(format->name));
			break;
		case 2:
			strscpy(format->name, "HD-DV", sizeof(format->name));
			break;
		default:
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d: unknown DV format %u\n",
			       dev->udev->devnum,
			       alts->desc.bInterfaceNumber, buffer[8]);
			return -EINVAL;
		}

		strlcat(format->name, buffer[8] & (1 << 7) ? " 60Hz" : " 50Hz",
			sizeof(format->name));

		format->fcc = V4L2_PIX_FMT_DV;
		format->flags = UVC_FMT_FLAG_COMPRESSED | UVC_FMT_FLAG_STREAM;
		format->bpp = 0;
		ftype = 0;

		/* Create a dummy frame descriptor. */
		frame = &format->frame[0];
		memset(&format->frame[0], 0, sizeof(format->frame[0]));
		frame->bFrameIntervalType = 1;
		frame->dwDefaultFrameInterval = 1;
		frame->dwFrameInterval = *intervals;
		*(*intervals)++ = 1;
		format->nframes = 1;
		break;

	case UVC_VS_FORMAT_MPEG2TS:
	case UVC_VS_FORMAT_STREAM_BASED:
		/* Not supported yet. */
	default:
		uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
		       "interface %d unsupported format %u\n",
		       dev->udev->devnum, alts->desc.bInterfaceNumber,
		       buffer[2]);
		return -EINVAL;
	}

	uvc_trace(UVC_TRACE_DESCR, "Found format %s.\n", format->name);

	buflen -= buffer[0];
	buffer += buffer[0];

	/* Parse the frame descriptors. Only uncompressed, MJPEG and frame
	 * based formats have frame descriptors.
	 */
	while (buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE &&
	       buffer[2] == ftype) {
		frame = &format->frame[format->nframes];
		if (ftype != UVC_VS_FRAME_FRAME_BASED)
			n = buflen > 25 ? buffer[25] : 0;
		else
			n = buflen > 21 ? buffer[21] : 0;

		n = n ? n : 3;

		if (buflen < 26 + 4*n) {
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d FRAME error\n", dev->udev->devnum,
			       alts->desc.bInterfaceNumber);
			return -EINVAL;
		}

		frame->bFrameIndex = buffer[3];
		frame->bmCapabilities = buffer[4];
		frame->wWidth = get_unaligned_le16(&buffer[5])
			      * width_multiplier;
		frame->wHeight = get_unaligned_le16(&buffer[7]);
		frame->dwMinBitRate = get_unaligned_le32(&buffer[9]);
		frame->dwMaxBitRate = get_unaligned_le32(&buffer[13]);
		if (ftype != UVC_VS_FRAME_FRAME_BASED) {
			frame->dwMaxVideoFrameBufferSize =
				get_unaligned_le32(&buffer[17]);
			frame->dwDefaultFrameInterval =
				get_unaligned_le32(&buffer[21]);
			frame->bFrameIntervalType = buffer[25];
		} else {
			frame->dwMaxVideoFrameBufferSize = 0;
			frame->dwDefaultFrameInterval =
				get_unaligned_le32(&buffer[17]);
			frame->bFrameIntervalType = buffer[21];
		}
		frame->dwFrameInterval = *intervals;

		/* Several UVC chipsets screw up dwMaxVideoFrameBufferSize
		 * completely. Observed behaviours range from setting the
		 * value to 1.1x the actual frame size to hardwiring the
		 * 16 low bits to 0. This results in a higher than necessary
		 * memory usage as well as a wrong image size information. For
		 * uncompressed formats this can be fixed by computing the
		 * value from the frame size.
		 */
		if (!(format->flags & UVC_FMT_FLAG_COMPRESSED))
			frame->dwMaxVideoFrameBufferSize = format->bpp
				* frame->wWidth * frame->wHeight / 8;

		/* Some bogus devices report dwMinFrameInterval equal to
		 * dwMaxFrameInterval and have dwFrameIntervalStep set to
		 * zero. Setting all null intervals to 1 fixes the problem and
		 * some other divisions by zero that could happen.
		 */
		for (i = 0; i < n; ++i) {
			interval = get_unaligned_le32(&buffer[26+4*i]);
			*(*intervals)++ = interval ? interval : 1;
		}

		/* Make sure that the default frame interval stays between
		 * the boundaries.
		 */
		n -= frame->bFrameIntervalType ? 1 : 2;
		frame->dwDefaultFrameInterval =
			min(frame->dwFrameInterval[n],
			    max(frame->dwFrameInterval[0],
				frame->dwDefaultFrameInterval));

		if (dev->quirks & UVC_QUIRK_RESTRICT_FRAME_RATE) {
			frame->bFrameIntervalType = 1;
			frame->dwFrameInterval[0] =
				frame->dwDefaultFrameInterval;
		}

		uvc_trace(UVC_TRACE_DESCR, "- %ux%u (%u.%u fps)\n",
			frame->wWidth, frame->wHeight,
			10000000/frame->dwDefaultFrameInterval,
			(100000000/frame->dwDefaultFrameInterval)%10);

		format->nframes++;
		buflen -= buffer[0];
		buffer += buffer[0];
	}

	if (buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE &&
	    buffer[2] == UVC_VS_STILL_IMAGE_FRAME) {
		buflen -= buffer[0];
		buffer += buffer[0];
	}

	if (buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE &&
	    buffer[2] == UVC_VS_COLORFORMAT) {
		if (buflen < 6) {
			uvc_trace(UVC_TRACE_DESCR, "device %d videostreaming "
			       "interface %d COLORFORMAT error\n",
			       dev->udev->devnum,
			       alts->desc.bInterfaceNumber);
			return -EINVAL;
		}

		format->colorspace = uvc_colorspace(buffer[3]);

		buflen -= buffer[0];
		buffer += buffer[0];
	}

	return buffer - start;
}