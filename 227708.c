static struct uvc_streaming *uvc_stream_by_id(struct uvc_device *dev, int id)
{
	struct uvc_streaming *stream;

	list_for_each_entry(stream, &dev->streams, list) {
		if (stream->header.bTerminalLink == id)
			return stream;
	}

	return NULL;
}