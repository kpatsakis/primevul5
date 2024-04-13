static int uvc_register_terms(struct uvc_device *dev,
	struct uvc_video_chain *chain)
{
	struct uvc_streaming *stream;
	struct uvc_entity *term;
	int ret;

	list_for_each_entry(term, &chain->entities, chain) {
		if (UVC_ENTITY_TYPE(term) != UVC_TT_STREAMING)
			continue;

		stream = uvc_stream_by_id(dev, term->id);
		if (stream == NULL) {
			uvc_printk(KERN_INFO, "No streaming interface found "
				   "for terminal %u.", term->id);
			continue;
		}

		stream->chain = chain;
		ret = uvc_register_video(dev, stream);
		if (ret < 0)
			return ret;

		/* Register a metadata node, but ignore a possible failure,
		 * complete registration of video nodes anyway.
		 */
		uvc_meta_register(stream);

		term->vdev = &stream->vdev;
	}

	return 0;
}